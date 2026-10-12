# Native Android diagnostic APK — Android.3

[Download BM64-Android-debug.apk](https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk)

Package: `com.narmanb.bm64`, version `1.0.0-android.3`, arm64-v8a, Android API28+.

This is the full native recompiled game build with additional startup diagnostics.
It contains no ROM. Install over Android.1/Android.2 to retain ROM and settings;
the package and signing certificate match.

The user reports Android.2 no longer crashes on Start but stays black. Android.3
logs the missing shader compilation and first-frame GPU/presentation stages.
**This is a diagnostic build, not a confirmed fix for the black screen.**

Tap Start, wait about 30 seconds, then Android Back → Return to launcher → Copy
Crash Report. Paste that report even if there was no crash. If Back does not respond,
use Android Home and reopen the launcher. Do not tap Start again before copying:
a new attempt clears the previous attempt's startup log.

Full Gradle build, existing real-ROM import/input/touch checks, APK v2 signature,
matching update certificate, JNI exports, ARM64 ABI, 16 KiB alignment and ROM
exclusion checks passed. New diagnostic markers are present in the packaged game
library. Device title screen/gameplay, audio, saves, lifecycle and FPS remain
unverified; root cause and repair of the current black screen await device logs.

SHA-256: `b6caa3753363c38146591613e2f8b5aef4ce4465a932029b59110e9804d260d5`.

Source/build status and instructions: [ANDROID_PORT.md](../docs/ANDROID_PORT.md).
This APK was built from the source at commit `478b2d8`. It is hosted here because
cloud release asset uploads are unavailable; later builds can move to GitHub Releases.
