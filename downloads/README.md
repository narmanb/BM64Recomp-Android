# Native Android test APK — Android.7

[Download BM64-Android-debug.apk](https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk)

Package `com.narmanb.bm64`, version `1.0.0-android.7`, arm64-v8a, Android API28+.
Size: 37,008,355 bytes. No ROM bundled. Install over earlier full builds to retain
ROM/settings; the signing certificate and package match.

Launcher **Graphics Settings** and native **Settings → Graphics → Resolution**
now offer higher direct RT64 rendering scales:

| Scale | Approximate height at the usual 240-line game output |
| --- | --- |
| 1x | 240p |
| 2x (default) | 480p |
| 3x | 720p |
| 4x | 960p |
| 6x | 1440p |
| Auto / Match screen | Scales to the output window |

Start with 3x or 4x, then try 6x if sustained performance stays acceptable. Dimensions
vary with the game's framebuffer and aspect ratio. Original texture/sprite detail
remains limited by the original assets. Defaults and gameplay timing are unchanged.

Higher scales and Auto disable supersampling rather than stacking more GPU memory
use. Saved supersampling preferences return when selecting 1x/2x again, including
native RmlUi programmatic Change events. Native controller navigation skips disabled
controls; compact Android resolution labels keep the row short. Desktop choices
remain unchanged through a revision-pinned, Android-only runtime adaptation.

Android.6 quality controls remain: supersampling for 1x/2x, capability-checked MSAA,
Automatic/Standard/High internal color precision and HUD placement. Immersive
fullscreen, customizable touch visibility/opacity, independent physical input and
complete diagnostic export remain available.

Full native Gradle build and host regression suite passed. Tests run actual renderer
selection, JSON round trips and settings bindings, including saved supersampling
across mode transitions and unchanged desktop policy. APK v2 signature, matching
update certificate, JNI, ARM64/16 KiB alignment and ROM exclusion checks passed.
New choices are packaged in launcher DEX, native library and in-game assets.
Independent final review found no remaining important issues. New resolution modes
still need visual/performance testing on RP5/S25+; no measured FPS claim is made.
The optional absent uploaded-ROM test is skipped this time; earlier real-ROM
acceptance results remain documented.

SHA-256: `39bdba6faeb693fcdc7fd900677a4675d6ecba8e11308112720d493b662cb8f3`.

Source/status/instructions: [ANDROID_PORT.md](../docs/ANDROID_PORT.md).
APK built from source commit `8a11fc1`. Hosted here because cloud release asset
uploads are unavailable; later builds can move to GitHub Releases.
