package com.narmanb.bm64.touch;
/** Connection based visibility, shared by View and host tests. */
public final class TouchVisibility {
    private TouchVisibility() {}
    public static boolean shown(TouchLayout.Visibility mode, boolean connected, boolean menu, boolean editing) {
        if(editing)return true;
        if(mode==TouchLayout.Visibility.NEVER || menu)return false;
        return mode==TouchLayout.Visibility.ALWAYS || !connected;
    }
}
