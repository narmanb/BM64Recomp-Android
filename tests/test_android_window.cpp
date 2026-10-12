// Run the real application window factory with SDL calls intercepted on the host.
// No Android device is available; this verifies the Vulkan/EGL ownership contract.
#include <SDL.h>
#undef main
#include <cassert>
#include <cstdio>
#include <stdexcept>

static Uint32 requestedFlags;
static SDL_Window* fakeWindow=reinterpret_cast<SDL_Window*>(uintptr_t(1));
extern "C" SDL_Window* SDL_CreateWindow(const char*,int,int,int,int,Uint32 flags) {
    requestedFlags=flags;return fakeWindow;
}
extern "C" Uint32 SDL_GetWindowFlags(SDL_Window*) {return requestedFlags;}
extern "C" const char* SDL_GetError() {return "test SDL error";}
struct SDL_SysWMinfo {SDL_version version;};
bool SDL_GetWindowWMInfo(SDL_Window*,SDL_SysWMinfo*) {return true;}
void exit_error(const char*,...) {throw std::runtime_error("window creation failed");}
namespace ultramodern {
    struct gfx_callbacks_t {using gfx_data_t=void*;};
    namespace renderer {struct WindowHandle {SDL_Window* window;};}
}
SDL_Window* window;
#include "window_factory.inc"
int main() {
    auto result=create_window(nullptr);
    assert(result.window==fakeWindow);
    // SDL's Android backend defaults an unspecified API to GLES, connecting an
    // EGL producer. Vulkan then fails with VK_ERROR_NATIVE_WINDOW_IN_USE_KHR.
    assert((requestedFlags&SDL_WINDOW_VULKAN)!=0 && "Android window must explicitly select Vulkan");
    assert((requestedFlags&SDL_WINDOW_OPENGL)==0);
    assert((requestedFlags&SDL_WINDOW_RESIZABLE)!=0);
    puts("Android Vulkan window factory passed");
}
