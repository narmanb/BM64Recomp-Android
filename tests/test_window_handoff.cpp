#include "android_window_handoff.h"
#include <cassert>
struct Window { int references=1; };
struct SwapChain {Window* held=nullptr;void setRenderWindow(Window* w){held=w;} };
int main(){Window w;SwapChain chain;
    bm64::android::handoff_window(&w,&chain,[](Window* p){--p->references;});assert(w.references==1&&chain.held==&w);
    bm64::android::handoff_window(&w,static_cast<SwapChain*>(nullptr),[](Window* p){--p->references;});assert(w.references==0);
}
