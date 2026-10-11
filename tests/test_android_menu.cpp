#include "android_menu_event.h"
#include <cassert>
int main() {
    auto press=bm64::android::menu_event(true),release=bm64::android::menu_event(false);
    assert(press.type==SDL_KEYDOWN&&release.type==SDL_KEYUP);
    assert(press.key.keysym.scancode==SDL_SCANCODE_ESCAPE);
    assert(press.key.keysym.sym==SDLK_ESCAPE&&release.key.keysym.sym==SDLK_ESCAPE);
    assert(press.key.state==SDL_PRESSED&&release.key.state==SDL_RELEASED);
    assert(!press.key.repeat); // menu action cannot depend on a physical remapping
}
