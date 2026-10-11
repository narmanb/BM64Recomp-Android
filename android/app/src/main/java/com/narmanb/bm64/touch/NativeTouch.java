package com.narmanb.bm64.touch;
/** Game library supplies these symbols; preview/editor has an explicit optional load. */
public final class NativeTouch {
    private NativeTouch() {}
    public static void setState(int mask,float[] axes) { try { nativeSetState(mask,axes); } catch(UnsatisfiedLinkError e) {} }
    public static void setActive(boolean active) { if(!active)clear(); }
    public static void clear() { try { nativeClearState(); } catch(UnsatisfiedLinkError e) {} }
    public static boolean isMenuOpen() { try { return nativeIsMenuOpen(); } catch(UnsatisfiedLinkError e) { return false; } }
    public static void requestMenuToggle() { try { nativeRequestMenuToggle(); } catch(UnsatisfiedLinkError e) {} }
    public static native void nativeSetState(int mask,float[] axes);
    public static native void nativeClearState();
    public static native boolean nativeIsMenuOpen();
    public static native void nativeRequestMenuToggle();
    public static native float[] nativePollPreview();
}
