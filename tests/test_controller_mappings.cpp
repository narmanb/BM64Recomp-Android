#include "recomp_input.h"
#include <SDL.h>
#include <cassert>
#include "controller_defaults.inc" // Extracted from the actual frontend, not a test copy.
int main() {
    const auto& m=recomp::default_n64_controller_mappings;
    assert(m.a[0].input_type==3&&m.a[0].input_id==SDL_CONTROLLER_BUTTON_A);
    assert(m.b[0].input_id==SDL_CONTROLLER_BUTTON_X);
    assert(m.z.size()==2&&m.z[0].input_type==4&&m.z[0].input_id==SDL_CONTROLLER_AXIS_TRIGGERLEFT+1);
    assert(m.z[1].input_id==SDL_CONTROLLER_AXIS_TRIGGERRIGHT+1);
    assert(m.r[0].input_id==SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    assert(m.start[0].input_id==SDL_CONTROLLER_BUTTON_START);
    assert(m.c_up[0].input_id==-(SDL_CONTROLLER_AXIS_RIGHTY+1));
    assert(m.c_down[0].input_id==SDL_CONTROLLER_AXIS_RIGHTY+1);
    assert(m.c_left[0].input_id==-(SDL_CONTROLLER_AXIS_RIGHTX+1));
    assert(m.c_right[0].input_id==SDL_CONTROLLER_AXIS_RIGHTX+1);
    assert(m.analog_left[0].input_type==4&&m.analog_left[0].input_id==-(SDL_CONTROLLER_AXIS_LEFTX+1));
    assert(m.analog_right[0].input_id==SDL_CONTROLLER_AXIS_LEFTX+1);
    assert(m.analog_up[0].input_id==-(SDL_CONTROLLER_AXIS_LEFTY+1));
    assert(m.analog_down[0].input_id==SDL_CONTROLLER_AXIS_LEFTY+1);
    assert(m.dpad_up[0].input_id==SDL_CONTROLLER_BUTTON_DPAD_UP);
    assert(m.dpad_down[0].input_id==SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    assert(m.dpad_left[0].input_id==SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    assert(m.dpad_right[0].input_id==SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    assert(m.toggle_menu[0].input_type==3&&m.toggle_menu[0].input_id==SDL_CONTROLLER_BUTTON_BACK);
}
