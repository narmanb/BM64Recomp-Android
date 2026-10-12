# First native Android debug APK

[Download BM64-Android-debug.apk](https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk)

Package: `com.narmanb.bm64`, version `1.0.0-android.1`, arm64-v8a, Android API28+.

This is the full native recompiled game build for device testing. It includes no ROM.
Install, select your compatible Bomberman 64 USA retail ROM in the Android launcher,
tap Start, then Start Game in the retained native frontend.

Build, real-ROM import tests, APK signature, JNI exports, ARM64 ABI and 16 KiB
alignment checks passed. Device gameplay, audio, saves, Vulkan recovery and FPS
remain unverified. If startup closes, reopen the launcher and Save crash report.

SHA-256: `75649b7f1ade839c2ced6d9ab590b40f07677d943525031bc002ff1fa56a734d`.

Source/build status and instructions: [ANDROID_PORT.md](../docs/ANDROID_PORT.md).
This artifact is the same APK built from the source at commit `320a57b`, with
build documentation at `1f334c5`. It is hosted in this directory because cloud
release asset uploads are unavailable; later builds can move to GitHub Releases.
