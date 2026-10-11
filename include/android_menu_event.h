#pragma once
#include <SDL.h>
namespace bm64::android {
// Escape is the frontend's unconditional menu action; physical bindings are independent.
inline SDL_Event menu_event(bool down) {
    SDL_Event ev{};
    ev.type=down ? SDL_KEYDOWN : SDL_KEYUP;
    ev.key.state=down ? SDL_PRESSED : SDL_RELEASED;
    ev.key.keysym.scancode=SDL_SCANCODE_ESCAPE;
    ev.key.keysym.sym=SDLK_ESCAPE;
    return ev;
}
}
