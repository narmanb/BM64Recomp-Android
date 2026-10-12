# Native Android test APK — Android.6

[Download BM64-Android-debug.apk](https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk)

Package `com.narmanb.bm64`, version `1.0.0-android.6`, arm64-v8a, Android API28+.
Size: 36,991,679 bytes. No ROM bundled. Install over earlier full builds to retain
ROM/settings; the signing certificate and package match.

Launcher **Graphics Settings** now includes:

- Supersampling / downsampling: Off, 2x, 4x (for 1x/2x resolution).
- MS anti-aliasing: Off, 2x, 4x, 8x, subject to actual GPU support.
- Framebuffer color precision: Automatic, Standard, High. Changes internal color
  precision, not display HDR. Unsupported High falls back to Standard at startup.
- HUD placement: Original, 16:9, Expand to screen edges.

Existing resolution, aspect ratio and rendering frame-rate choices remain.
Defaults stay unchanged. For a sharper image try Match window resolution first;
for supersampling try 2x resolution with 2x downsampling. Higher settings can
reduce performance. Precision applies on the next game launch. Gameplay timing
is unchanged.

Android Vulkan checks required high-precision format features and sampled/color/
storage image usages before selecting it. MSAA checks intersect image/global
limits and custom sample-location support. The native menu enables exact supported
counts, including sparse 4x-only masks. Desktop behavior is preserved.

The user reports the prior installation working. New quality modes still require
visual/performance testing on RP5 and S25+; no measured FPS claim is made.
Immersive fullscreen, customizable touch controls, saved visibility/opacity,
independent physical input and complete raw diagnostic export remain available.

Full native Gradle build and host regressions passed, including unsupported color
precision fallback, actual Vulkan image support decisions and sparse MSAA choices.
The optional uploaded-ROM acceptance test is skipped in this resumed environment;
previous real-ROM acceptance results remain documented. APK v2 signature, matching
update certificate, JNI/settings bridge, ARM64 and 16 KiB alignment/ROM exclusion
checks passed. Independent final review found no remaining important issues.

SHA-256: `eb9543cafd276f0bab3f42cf1c0f68c8fab9523c6c162c963c7e37708c8a3195`.

Source/status/instructions: [ANDROID_PORT.md](../docs/ANDROID_PORT.md).
APK built from source commit `bb845bc`. Hosted here because cloud release asset
uploads are unavailable; later builds can move to GitHub Releases.
