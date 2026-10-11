// Adapted from Goemon64Recomp-Android v1.0.8 (GPL-3.0); see android/NOTICE.md.
package com.narmanb.bm64.touch;

/** Logical N64 inputs; physical SDL remapping remains independent. */
public enum TouchControl {

    /** Analog stick. Drives an axis pair rather than a button. */
    STICK(Kind.STICK, "", Dir.NONE, N64.NONE),

    /** N64 A — place bomb. The big one on a real pad, so it gets the largest default radius. */
    A(Kind.BUTTON, "A", Dir.NONE, N64.BUTTON_A),
    /** N64 B — lift bomb/talk. */
    B(Kind.BUTTON, "B", Dir.NONE, N64.BUTTON_B),

    C_UP(Kind.BUTTON, "C", Dir.UP, N64.BUTTON_C_UP),
    C_RIGHT(Kind.BUTTON, "C", Dir.RIGHT, N64.BUTTON_C_RIGHT),
    C_DOWN(Kind.BUTTON, "C", Dir.DOWN, N64.BUTTON_C_DOWN),
    C_LEFT(Kind.BUTTON, "C", Dir.LEFT, N64.BUTTON_C_LEFT),

    /** N64 Z — detonate bomb. Touch sends the logical N64 button directly. */
    Z(Kind.SHOULDER, "Z", Dir.NONE, N64.BUTTON_Z),
    /** N64 L — debug input, also available to mods. */
    L(Kind.SHOULDER, "L", Dir.NONE, N64.BUTTON_L),
    /** N64 R — stop bomb. Right shoulder in the stock mapping. */
    R(Kind.SHOULDER, "R", Dir.NONE, N64.BUTTON_R),

    START(Kind.BUTTON, "START", Dir.NONE, N64.BUTTON_START),

    /**
     * Opens the recomp's own settings menu — the on-screen stand-in for Select.
     * Without it, a device with no gamepad could never reach Settings, because
     * Select appears nowhere else on the overlay.
     */
    MENU(Kind.MENU, "", Dir.NONE, N64.NONE),
    D_UP(Kind.BUTTON, "D", Dir.UP, 11),
    D_RIGHT(Kind.BUTTON, "D", Dir.RIGHT, 8),
    D_DOWN(Kind.BUTTON, "D", Dir.DOWN, 10),
    D_LEFT(Kind.BUTTON, "D", Dir.LEFT, 9);

    /** N64 button bit indices and the two native touch stick axes. */
    public static final class N64 {
        private N64() {}

        public static final int NONE = -1;

        // N64 controller button bit indices
        public static final int BUTTON_A = 15;
        public static final int BUTTON_B = 14;
        public static final int BUTTON_Z = 13;
        public static final int BUTTON_R = 4;
        public static final int BUTTON_START = 12;
        public static final int BUTTON_L = 5;
        public static final int BUTTON_C_UP = 3;
        public static final int BUTTON_C_DOWN = 2;
        public static final int BUTTON_C_LEFT = 1;
        public static final int BUTTON_C_RIGHT = 0;

        // Touch stick axis array
        public static final int AXIS_LEFTX = 0;
        public static final int AXIS_LEFTY = 1;

        /** Length of the axis array the native side expects. */
        public static final int AXIS_COUNT = 2;
    }

    /** How a control is drawn and hit-tested. */
    public enum Kind {
        /** Round button, one circular hit target. */
        BUTTON,
        /** Rounded rectangle along a screen edge — L, Z and R. */
        SHOULDER,
        /** Analog stick: a base ring with a knob that follows the finger. */
        STICK,
        /** The settings handle: drawn as a hamburger rather than a glyph. */
        MENU
    }

    /** Which way a control's arrow points, for the C cluster. */
    public enum Dir { NONE, UP, RIGHT, DOWN, LEFT }

    public final Kind kind;
    public final String label;
    public final Dir dir;
    /** N64 bit index this control presses, or {@link N64#NONE} if it drives an axis. */
    public final int buttonBit;

    TouchControl(Kind kind, String label, Dir dir, int buttonBit) {
        this.kind = kind;
        this.label = label;
        this.dir = dir;
        this.buttonBit = buttonBit;
    }

    /** True if this control is one of the four C-cluster members. */
    public boolean isCButton() {
        return this == C_UP || this == C_RIGHT || this == C_DOWN || this == C_LEFT;
    }

    /** The C cluster in diamond order: up, right, down, left. */
    public static final TouchControl[] C_CLUSTER = { C_UP, C_RIGHT, C_DOWN, C_LEFT };
}
