# Native Android architecture

Goal: preserve BM64's static recompilation, game patches, desktop build and original
simulation timing while adding ARM64 Android packaging, system Vulkan/RT64 and
independent physical/touch input. Primary RP5 Android 13; secondary S25+ Android 16.

Keep upstream runtime/frontend pins. Apply reviewed Android-only dependency patches
with revision checks; retain BM64's rectangle-aspect extended command. Separate host
N64Recomp/RSPRecomp/file_to_c and MIPS patch generation from NDK compilation.

A Java launcher imports and verifies the retail USA ROM through SAF into private
storage. Generation consumes the separate decompressed USA ROM specified by upstream.
No ROM or generated game bytes enter Git or APK assets. A separate game process hosts
SDLActivity/libBM64, isolating crashes from the launcher. Export local diagnostics.

Reuse Goemon v1.0.8 vector overlay, pointer state machine and normalized layout editor.
Adapt outputs to logical N64 buttons independently of physical remapping. Physical
pads remain in upstream SDL controller/event/binding paths. OR button states; select
a whole analog vector by magnitude to preserve direction rather than canceling inputs.
Use a native short-tap latch. Clear touch state on hide/cancel/focus loss/pause.

An atomic private file shared by launcher and game owns touch settings: Always On, Automatic (connected controller),
Completely Off (no intercepts), 10–100% opacity/default 60%, layout, size, sensitivity,
deadzone and optional haptics. Launcher always exposes settings and layout editor.
Automatic uses Android InputManager add/change/remove callbacks, including initial
enumeration. Opacity never changes hit-testing or physical input.

Native graphics settings use upstream JSON/config APIs; force Vulkan on Android.
Preserve widescreen/interpolation patches and simulation timing. Surface recreation,
audio interruption and background simulation require device validation, not assumptions.

Without generated game sources, build a distinct foundation APK for ROM/settings/input
validation. It must identify itself as non-playable, exclude Start Game execution, and
never substitute a fake game/renderer. Full game build fails with exact missing inputs.

Validation: ROM normalization/rejection/atomic import, multitouch/release and layout
round trips, visibility transitions, native short-tap/clear/merge, host-vs-target tools,
JNI exports, APK ARM64/signature/16 KiB alignment/no ROM. Title screen, gameplay,
audio, saves, GPU/lifecycle and performance remain unverified until device testing.
