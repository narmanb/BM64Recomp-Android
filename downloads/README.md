# Native Android debug APK — Android.2

[Download BM64-Android-debug.apk](https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk)

Package: `com.narmanb.bm64`, version `1.0.0-android.2`, arm64-v8a, Android API28+.

This is the full native recompiled game build for device testing. It includes no ROM.
Install over Android.1 to retain your imported ROM and settings: the package and
signing certificate match. For a new installation, select your compatible Bomberman
64 USA retail ROM, tap Start, then Start Game in the native frontend.

Android.2 fixes the SDL/EGL native-window ownership conflict behind the reported
Galaxy S25+ `VK_ERROR_NATIVE_WINDOW_IN_USE_KHR` startup failure. The graphics settings
dialog now scrolls in landscape. Both changes still need a physical-device retest.

Full Gradle build, real-ROM import/input tests, production Vulkan window regression,
APK signature, JNI exports, ARM64 ABI and 16 KiB alignment checks passed. Successful
device gameplay, audio, saves, Vulkan recovery and FPS remain unverified. If startup
closes, reopen the launcher and Save crash report; confirm Android.2 in the report.

SHA-256: `db61050879975853daacb9132b7dbf003cdb0adf387d79640cb51159acc070da`.

Source/build status and instructions: [ANDROID_PORT.md](../docs/ANDROID_PORT.md).
This APK was built from the source at commit `3e7715d`. It is hosted here because
cloud release asset uploads are unavailable; later builds can move to GitHub Releases.
