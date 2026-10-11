#include "plume_android_surface.h"
#include <cassert>
#include <atomic>
struct Window {int refs=1;};
int main() {
    Window old,next;Window* current=&old;std::atomic<Window*> pending{&next};
    auto release=[](Window* w){--w->refs;};
    int surface=12,attempts=0,queries=0;
    auto resize=[&] {
        if(plume::androidSurfaceNeedsRetry(pending.load(),surface)) {
            auto window=plume::adoptAndroidWindow(current,pending.exchange(nullptr),release);
            assert(window==&next&&next.refs==1);surface=0;
            if(++attempts==1)return false; // vkCreateAndroidSurfaceKHR transient failure
            surface=23;
        }
        assert(surface!=0);++queries;return true;
    };
    assert(!resize());assert(queries==0&&old.refs==0&&next.refs==1);
    assert(resize());assert(attempts==2&&queries==1&&next.refs==1);
    assert(resize());assert(attempts==2&&queries==2);
    release(current);assert(next.refs==0);
}
