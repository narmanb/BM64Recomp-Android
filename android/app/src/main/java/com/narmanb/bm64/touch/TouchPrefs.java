// Adapted from Goemon64Recomp-Android v1.0.8 (GPL-3.0); see android/NOTICE.md.
package com.narmanb.bm64.touch;

import android.content.Context;
import android.util.Log;
import java.io.*;

/** App-private, uncached settings shared safely with the separate SDL process. */
public final class TouchPrefs {
    private TouchPrefs() {}
    private static File file(Context context) {
        return new File(context.getFilesDir(), "bm64/touch_controls.txt");
    }
    public static TouchLayout load(Context context) { return TouchStore.load(file(context)); }
    public static void save(Context context, TouchLayout layout) {
        try {TouchStore.save(file(context),layout);}
        catch(IOException e) {Log.e("BM64", "Cannot persist touch layout",e);}
    }
}
