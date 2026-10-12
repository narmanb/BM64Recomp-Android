# Native Android test APK — Android.4

[Download BM64-Android-debug.apk](https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk)

Package: `com.narmanb.bm64`, version `1.0.0-android.4`, arm64-v8a, Android API28+.
Size: 36,975,043 bytes. No ROM bundled. Install over earlier full builds to retain
ROM/settings: the signing certificate and package match.

Android.4 fixes inconsistent Android swapchain/UI/presentation formats and keeps
EXCLUSIVE shared GPU resources in one Android queue family. It stops on fatal GPU
submission/fence/pipeline/descriptor failures and saves the first error, rather than
continuing invalid frames indefinitely. Desktop graphics policies are preserved.

Copied reports now keep the beginning and end of large logs. ZIP exports also
contain **complete raw logs** and graphics configuration; earlier APKs included
only a truncated text preview plus historical traces. They exclude ROM/assets.

The supplied S25+ Android.3 report showed device loss after recording the first UI
frame but omitted the initial error. **Android.4 needs a physical-device retest;
these source corrections are not proof that the black screen is resolved.**

Install as an update, tap Start, then Start Game if the native menu appears. If it
stops or stays black, return to the launcher and Save crash report; send the ZIP.
Existing Android.3 logs can also be exported before pressing Start again. A new
attempt clears prior startup/fatal logs. If Back does not respond, use Android Home
and reopen the launcher to save the report.

Full Gradle build, real-ROM import/input/touch tests, Android/desktop renderer
contract tests, fatal-error subprocess tests, complete archive tests, APK v2
signature, update certificate, JNI/ARM64 ABI, ELF/ZIP16 KiB alignment and ROM
exclusion checks pass. Successful device gameplay, audio, saves, lifecycle and FPS
remain unverified. Independent code review found no blocking issues.

SHA-256: `ad4a0868456b8925830a730e4d33a2ac318ead888f4d4573a7fc29b3f071784c`.

Source/status/instructions: [ANDROID_PORT.md](../docs/ANDROID_PORT.md).
APK built from source commit `7882287`. Hosted here because cloud release asset
uploads are unavailable; later builds can move to GitHub Releases.
