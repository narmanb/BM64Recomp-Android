#include "android_pad.h"
#include <cassert>
#include <limits>
int main() {
    bm64::android::Pad pad;
    pad.set(0x8000,.5f,-.25f);pad.set(0,.5f,-.25f);
    auto s=pad.poll();assert(s.buttons==0x8000 && s.x==.5f && s.y==-.25f);
    assert(pad.poll().buttons==0);
    pad.set(0x4000,1,1);pad.clear();s=pad.poll();assert(s.buttons==0&&s.x==0&&s.y==0);
    pad.set(0xffff,std::numeric_limits<float>::quiet_NaN(),3);s=pad.poll();assert(s.x==0&&s.y==1);
    auto m=bm64::android::merge({0x8000, .5f,0},{0x4000,-.25f,.1f});
    assert(m.buttons==0xc000&&m.x==.5f&&m.y==0);
    m=bm64::android::merge({1,.25f,.1f},{2,-.7f,-.2f});assert(m.buttons==3&&m.x==-.7f&&m.y==-.2f);
    auto shaped=bm64::android::shape({0,.3f,.4f},0);assert(shaped.x<.3f&&shaped.y<.4f&&std::abs(shaped.x/shaped.y-.75f)<.001f);
    assert(bm64::android::shape({0,.3f,.4f},100).x==.3f);
    pad.clear();pad.set(0x1000,0,0);assert(pad.poll().buttons==0x1000);pad.set(0,0,0);assert(pad.poll().buttons==0);
}
