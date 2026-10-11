#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <mutex>

namespace bm64::android {
struct State { uint16_t buttons = 0; float x = 0, y = 0; };
// Snapshot once per N64 poll; short button taps survive exactly one poll.
class Pad {
    std::mutex mutex;
    State current{};
    uint16_t pending = 0;
public:
    void set(uint16_t buttons, float x, float y) {
        std::lock_guard lock(mutex);
        pending |= buttons & ~current.buttons;
        current={buttons, std::isfinite(x) ? std::clamp(x,-1.f,1.f) : 0.f,
                          std::isfinite(y) ? std::clamp(y,-1.f,1.f) : 0.f};
    }
    State poll() {
        std::lock_guard lock(mutex);
        State ret=current;ret.buttons |= pending;pending=0;return ret;
    }
    void clear() { std::lock_guard lock(mutex);current={};pending=0; }
};
inline State merge(State physical, State touch) {
    State result=physical;
    result.buttons |= touch.buttons;
    if(touch.x*touch.x+touch.y*touch.y > physical.x*physical.x+physical.y*physical.y) {
        result.x=touch.x;result.y=touch.y;
    }
    return result;
}
inline State shape(State state,int sensitivity) {
    const float magnitude=std::hypot(state.x,state.y);
    if(magnitude>0.f && magnitude<1.f && sensitivity<100) {
        const float exponent=1.f+(1.f-std::clamp(sensitivity,0,100)/100.f)*1.5f;
        const float ratio=std::pow(magnitude,exponent)/magnitude;
        state.x*=ratio;state.y*=ratio;
    }
    return state;
}
bool is_foreground();
int physical_sensitivity();
void poll_touch();
State touch_state();
void clear_touch();
}
