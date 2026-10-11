package com.narmanb.bm64.touch;
import android.view.InputDevice;
public final class ControllerPresence {
    private ControllerPresence() {}
    public static boolean connected() {
        for(int id:InputDevice.getDeviceIds()) {
            InputDevice d=InputDevice.getDevice(id);
            if(d!=null && !d.isVirtual() && ((d.getSources()&InputDevice.SOURCE_GAMEPAD)==InputDevice.SOURCE_GAMEPAD
                || (d.getSources()&InputDevice.SOURCE_JOYSTICK)==InputDevice.SOURCE_JOYSTICK))return true;
        }
        return false;
    }
}
