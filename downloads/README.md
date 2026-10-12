# Native Android test APK — Android.5

[Download BM64-Android-debug.apk](https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk)

Package: `com.narmanb.bm64`, version `1.0.0-android.5`, arm64-v8a, Android API28+.
Size: 36,991,679 bytes. No ROM bundled. Install over earlier full builds to retain
ROM/settings: signing certificate and package match.

Android.5 keeps the game in immersive fullscreen and restores it after dialogs
and resume. Edge swipes can temporarily reveal Android navigation. SDL window
flags match the activity mode; UI fullscreen dispatch never waits on its own
resize. Native callers retain their synchronization. Window Mode is hidden in
Android's native Graphics menu; desktop behavior is preserved.

Open **native Settings → Controls → Touchscreen controls**, launcher Touchscreen
Settings, or Android Back → Touchscreen Settings. Always On / Automatic /
Completely Off and saved10–100% opacity (default60%) remain available. The slider
shows its current percentage live. Physical input stays independent. Launcher
graphics saving preserves MSAA choices.

User screenshots after Android.4 show the native frontend rendering on the S25+.
**Android.5 immersion/settings need a device retest; original game title/gameplay,
audio, saves, lifecycle recovery and performance remain unverified.** Downsampling
is higher-resolution rendering followed by reduction (additional GPU work). MSAA
is wired to RT64 and capability-gated; its in-game quality/performance need testing.

Full native Gradle build, existing ROM/input/touch tests, fullscreen window and
UI/native dispatch regressions, APK v2 signature, matching update certificate,
JNI/Java settings bridge, ARM64 and16 KiB alignment/ROM exclusion checks pass.
Independent review finding about UI-thread waiting was fixed; final review passed.
Complete raw ZIP diagnostics and Android.4 Vulkan/fatal-failure corrections remain.

SHA-256: `d633f63cf85f69faa538f08ce5cf75c42746007811892fdde3b3794ddd20c67b`.

Source/status/instructions: [ANDROID_PORT.md](../docs/ANDROID_PORT.md).
APK built from source commit `27ca8d7`. Hosted here because cloud release asset
uploads are unavailable; later builds can move to GitHub Releases.
