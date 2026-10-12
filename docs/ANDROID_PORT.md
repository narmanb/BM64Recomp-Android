# Bomberman 64 Android port — development status

Branch: `android-port` in **narmanb/BM64Recomp-Android**. Started from `master`
`726adc908ca7abd27c4cba12d1f52a0c243727df`, identical to RevoSucks/BM64Recomp
master at audit. Upstream and master are not modified. This is static recompilation,
not an emulator. No ROM, extracted assets or generated game binary is committed.

## Current status

The **full native ARM64 debug APK now builds** directly in this cloud environment
(updated 2026-10-12). Package `com.narmanb.bm64`, version `1.0.0-android.5`, minimum API28.
The game/RSP/patch code is generated and linked into `libBM64.so`; Vulkan/RT64,
SDL2 and the Android launcher/control system are packaged. Start is enabled after
importing a compatible retail ROM. The ROM is **not bundled**.

APK SHA-256: `d633f63cf85f69faa538f08ce5cf75c42746007811892fdde3b3794ddd20c67b`.
This is an Android fullscreen/settings update, **not verified playable/stable**.
No Android device is attached here. The separate ROM-free foundation APK remains
available for launcher/settings/control-preview testing, with Start disabled.

Implemented source:

- API28+, ARM64-only Gradle/NDK build, distinct non-playable foundation package;
  full game target consumes host-generated game/RSP/patch sources and fails early
  when they are absent. Desktop remains an executable.
- Android SAF retail ROM import/validation with raw/ZIP z64/v64/n64 support;
  validate before atomic replacement of `files/bm64/bm64_us.z64`, the runtime's
  expected stored filename. Saves/config/assets stay under private `files/bm64`.
- Separate `:game` SDL process and native startup checkpoints/stdout/stderr,
  persisted Java failures and diagnostic ZIP export; historical system native
  tombstone/ANR trace included when Android makes it available.
- Launcher graphics defaults and physical mapping/deadzone/sensitivity settings.
  Upstream SDL integrated/USB/Bluetooth detection, event-driven menus and native
  in-game remapping are retained. Stock mappings: South=A place bomb, West=B
  lift/talk, triggers=Z detonate, RB=R stop bomb, right stick=C camera/zoom,
  D-pad=menu directions, Start=pause, Select=settings. Stick is proportional.
- Goemon v1.0.8 vector touch overlay/multitouch/editor adapted to **logical N64**
  inputs so physical remapping does not change touchscreen labels. Includes
  analog, A/B/Z/R/L, C buttons, D-pad, Start and settings handle.
- Touch settings: **Always On**, **Automatic** (connected physical controller),
  **Completely Off** (all gameplay hitboxes/input disabled); connection callbacks
  update without a restart. Physical inputs remain independent.
- Opacity 10–100%, default60%, independent of visibility; immediate preview and
  automatic persistence. Individual positions/sizes including C buttons, joystick
  size, sensitivity/deadzone, reset and optional haptics. Atomic private-file settings
  reload across launcher and game processes without SharedPreferences cache staleness.
  The launcher exposes
  settings and an editor even with touch controls off.
- Native button OR/whole-vector stick arbitration, one-poll short-tap latch,
  release/clear on hiding, cancellation, focus loss, pause and edit transitions.
  Native gameplay input is neutral while the game activity is paused/unfocused,
  independently of touch visibility.
- Reviewed RT64/Plume Android patches keep BM64's existing dependency revisions
  and rectangle-aspect command. Host DXC/file_to_c, stock Adreno compute inlining,
  SDL native window, surface replacement with owned-reference handoff and failed
  creation retries (including same-size windows), and private
  RT64 storage (`detectDataPath=false`, `files/bm64/rt64`). No custom GPU driver.

**User screenshots confirm visible native frontend rendering on the S25+. The
original game title screen, gameplay, audio, saves, lifecycle recovery and performance
remain unverified on physical hardware.**

## Android.2 startup and settings fixes

The user tested Android.1 on a Samsung Galaxy S25+ (SM-S936U1, Android16/API36,
stock Adreno830). ROM selection worked, but Start returned to the launcher.
The current-attempt log reached RT64 initialization and reported
`vkCreateAndroidSurfaceKHR failed with error code 0xC46535FF`, which is
`VK_ERROR_NATIVE_WINDOW_IN_USE_KHR` (-1000000001). Older historical crash entries
also appeared; no current native tombstone was supplied for those crashes.

SDL2 defaults an Android window with no explicit graphics backend to OpenGL and
creates an EGL surface. That EGL surface claims the native window before RT64
creates its Vulkan surface. Android.2 explicitly sets `SDL_WINDOW_VULKAN` in the
application window factory and RT64's fallback, retaining the separate Plume
`ANativeWindow*` interface. It logs actual SDL window flags for the next device
report. No driver replacement or simulation timing change is involved.
A host regression runs the real window factory with intercepted SDL calls: it
failed without the fix and now confirms Vulkan, no OpenGL and resizable flags.
The disabled Android desktop-preload path no longer emits a misleading failure.

The graphics settings content now sits in a ScrollView, with Save/Cancel outside
its scrolling area, so small landscape screens can reach every option. Full
Gradle build and APK checks passed. Android.2 has the same signing certificate and
package as Android.1; install as an update to retain imported ROM and settings.
**The updated surface creation and scrolling still require a device retest.**

## Android.2 black-screen investigation

The user reports Android.2 no longer exits/crashes on Start on the Galaxy S25+,
but remains at a black screen indefinitely. This confirms an improvement over
the earlier surface-ownership crash, **not successful rendering or gameplay**.
The black-screen attempt's current startup report has been requested; its stopping
point is not yet known. ROM selection worked in the previous test. No physical
device is connected to this environment, so the stall cannot be reproduced here.

Android.3 adds diagnostic instrumentation without changing graphics configuration,
synchronization, simulation timing or the game-start sequence. RT64 logs setup
phases and named startup shader pipeline calls with monotonic timestamps and
native thread IDs. It traces the first queued present and only the first three
rendered frames: image acquisition, UI drawing, GPU submission/fence wait and
presentation. Frontend checkpoints distinguish renderer-ready, first screen
update and first UI layout/geometry/composition. The Android-private stdout/stderr
log captures these entries; launcher Copy Crash Report works even without a crash.
Pipeline "returned" records call completion, not successful compilation; inspect
nearby Vulkan error messages as well. Successful presentation return is not proof
of visible or correct game content. Desktop logging is disabled.

Android.3 Gradle `assembleDebug` succeeded in 2m12s. APK v2 signature, matching
update certificate, ARM64/JNI/16 KiB alignment/ROM exclusion checks and existing
real-ROM/input/touch host checks passed. The packaged game library contains all
new diagnostic markers. Independent review replayed all 18 RT64 patch paths against
the pinned revision and found no important issues. No device result for this build
is available yet.

**Android.3 is a diagnostic build, not a confirmed fix for the black screen.**
Install it as an update, Start, wait about 30 seconds, then use Android Back →
Return to launcher → Copy Crash Report. If the activity does not respond, return
through Android's home screen and reopen the launcher to copy the existing log;
do not tap Start again before copying because that clears the attempt's log.
Do not replace the driver or change several graphics settings during this capture.
The report will determine whether initialization, shader linking, UI rendering,
GPU work, swapchain presentation or a later step needs correction.

## Android.3 device report comparison and Android.4 corrections

On 2026-10-12 the user supplied both copied text and a diagnostic ZIP from the
Galaxy S25+ running Android.3. They agree on startup checkpoints: RT64 setup,
renderer-ready, first screen update and first UI layout/geometry/composition all
completed. The ZIP contains a 67,700-byte `crash-report.txt` and a 421,195-byte
**historical** tombstone from Android.1 on October11. It does not contain the raw
current native log. Its included native log is exactly the last 65,536 bytes,
starting in the middle of an error; it contains 578 `vkQueueSubmit` device-lost
errors (`0xFFFFFFFC`, `VK_ERROR_DEVICE_LOST`), 579 fence timeouts (`0x2`), 207 later
swapchain native-window-in-use errors, one surface-lost error and a later surface
recreation. Some later entries are absent from the pasted portion. The older
SIGSEGV trace is not evidence of a current Android.3 crash. The first error and
initial shader/device/presentation diagnostics are missing from **both** previews.

The exporter reused the bounded copy text for the ZIP. Android.4 corrects this:
Copy now retains the first 48 KiB and last16 KiB of large native logs and clearly
marks omitted bytes; Save streams complete raw allowlisted diagnostics and graphics
configuration to separate ZIP entries. ROM and asset directories are excluded.
Each streamed file has a fixed initial length, so export does not chase a live log
forever. A large-error-log regression verifies preserved first/latest errors and
byte-for-byte complete ZIP data. Existing Android.3 logs remain in app-private
storage when installing this update; they can be exported before a new Start.

Source review also identified Vulkan contract defects that Android.4 corrects:

- The Android format helper already returns RGBA for RT64's final VI pipelines,
  but the application swapchain and frontend UI still requested BGRA. Both now
  use `GetSwapChainFormat()`, including UI sample-capability queries and offscreen
  textures. This matches the compatible Goemon implementation. Android and desktop
  regressions compile the real production decisions and check consistency; desktop
  still uses BGRA. Launcher VI registers are initially blank, so this mismatch is
  **not proof** of the first menu-frame device loss.
- Plume created EXCLUSIVE resources shared by RT64/UI while independently selecting
  DIRECT, COMPUTE and COPY families, with no family ownership transfers. On Android,
  all three now use the graphics/compute/transfer-capable family; virtual queues and
  existing synchronization remain. Offered/selected families are logged. Synthetic
  dedicated-transfer/compute and single-family cases validate the real selection
  code; the desktop policy is retained. The S25+'s original families were absent
  from the truncated log, so this is not a measured diagnosis of that device.
- Failed submits and fence waits previously returned void and let frame generation
  continue, producing the observed error storm and misleading subsequent stages.
  Android now saves the first fatal GPU error and exits only the isolated game
  process, without attempting GPU cleanup waits. Required shader/pipeline/layout/
  renderpass/descriptor creation failures follow the same policy rather than leaving
  null required objects for later draws. The launcher displays Graphics stopped.
  A subprocess regression runs the actual submit/fence methods with injected Vulkan
  results: loss/timeout stops execution, success still waits/resets correctly.

**The first S25+ GPU-loss trigger is not established by the supplied truncated
report. Android.4 fixes confirmed source/reporting defects and requires physical
retesting; it is not a verified playable or stable release.** No driver replacement,
game simulation/timing change, controller/touch behavior change or measured FPS
result is involved. Further GPU failure should return to the launcher with a
complete startup report rather than continue indefinitely at a black screen.

## Android.5 immersive display and touchscreen settings

User screenshots following Android.4 delivery show the native launcher and native
Graphics menu rendering on the S25+. This is evidence of **visible frontend
rendering**, not entry into the original game's title screen or gameplay. Status
and three-button navigation bars remained visible and obstructed the native menu.

SDL's queued initial `setWindowStyle(false)` overrides the activity's earlier
one-time hide flags. Android.5 creates the actual SDL window with Vulkan and
FULLSCREEN_DESKTOP flags, maintains SDL fullscreen state, hides system bars through
API30 WindowInsetsController (legacy API28 fallback), and restores immersion on
resume/focus after dialogs. Android's edge swipe can reveal transient navigation;
it is not disabled. SDL's vendored Java shim has an explicitly marked modification:
UI callers dispatch style changes without waiting for a resize that their own
thread must process; native callers retain the bounded resize wait. The production
window-factory regression previously failed without fullscreen. The Java regression
extracts the actual SDL dispatch method: it previously blocked the UI for500ms,
now returns promptly, and still tests native wait/notification.

The native Graphics menu hides Window Mode on Android and routes controller focus
between aspect ratio and rendering rate. Old Windowed JSON preferences are coerced
to Fullscreen on Android; the fullscreen hotkey is a no-op there. Desktop options
and mode behavior remain available.

Native **Settings → Controls → Touchscreen controls** now opens the existing
Android settings dialog through a Java/native bridge on the UI thread. Launcher
Touchscreen Settings and Android Back recovery remain available when controls
are Completely Off. Existing Always On / Automatic / Completely Off modes,
independent physical input and saved10–100% opacity/default60% are retained. The
opacity slider now displays its current percentage and updates it immediately.
Launcher graphics saving preserves the user's MSAA setting instead of silently
resetting it to None.

Downsampling renders at higher internal resolution and reduces the image to the
selected output resolution: smoother edges/detail with additional GPU work. It is
available for Original and Original2x. Start with Off on mobile, then assess on
device. MSAA is connected to RT64's multisampling configuration and updates;
unsupported modes are disabled based on renderer/device capabilities. Its visual
quality, stability and performance in Bomberman64 gameplay on either target device
are **not yet verified**; the screenshot alone is not a multisampling test.

Full Gradle build, regression suite and signature/ARM64/JNI/16 KiB/ROM exclusion
checks pass. Both native callback and Java method are present in the APK (Java in
classes2.dex), and the updated Android assets are packaged. Independent review's
UI-thread wait finding is fixed; final review found no remaining important issues.
The update uses the same signing certificate and preserves ROM/settings. Physical
Android.5 bar hiding/restoration, native touch-settings entry and slider presentation
still need confirmation.

## Audited references and dependency pins

| Source | Revision |
| --- | --- |
| Hero Android history (`docs/ANDROID_PORT.md`) | `dfb46c5510f7ba982411ae9567a8d6cba03a10c2` |
| Goemon touchscreen tag v1.0.8 | `367b1017907d08867d70a4b2e9730b21d75577ff` |
| Goemon repository current audit | `7c6a59848d2b217a2f6dde404e8dbd8534686a71` |
| BM64 symbols | `9578623fb2d41616437654762e96af48ad0063a2` |
| BM64 N64ModernRuntime | `0bb76b0fc7078cdec37ec3d220af4aaf8212c611` |
| BM64 RT64 | `71b5906f0b991459744dabfcc68d04616ac71797` |
| BM64 Plume | `51b1ad443b9f202c5cfc930ae25345d3f2ba7716` |
| Goemon Android RT64 source for adaptations | `5bec328c15ab02ab70154d32aa367f3cdb83ab38` |
| Goemon Android Plume source for adaptations | `e12bec2f14d67245dc388fa6926148bfef149142` |
| Host recompiler (upstream CI pin) | `98bf104b1b5ed83126af8bcab0cc964782617dbf` |
| SDL (Java/native 2.32.8) | `98d1f3a45aae568ccd6ed5fec179330f47d4d356` |
| FreeType 2.13.3 | `534ad3456055ee1f65ecde3bcf22a656a31514d1` |

GPL-3.0 main/reference code, zlib SDL, MIT RT64/Plume; see `android/NOTICE.md`.
Hero's frontend submodule is not reused wholesale: BM64 owns frontend code here.
Goemon's reveal-on-touch Automatic mode, SDL touch mappings and rectangle heuristics
are not copied; BM64's controller-independent touch labels and graphics patches stand.

## ROM inputs and source generation

Runtime/import: **Bomberman 64 (USA)** retail, 8 MiB, normalized big-endian SHA-1:
`8a7648d8105ac4fc1ad942291b2ef89aeca921c9`.
Reference: libretro/libretro-database No-Intro Nintendo64 DAT (NBME).
Runtime additionally checks upstream XXH3 `c4c0b74bbb696426`.

Host generation: follow upstream `BUILDING.md`, build
<https://github.com/Bomberhackers/bm64> with your own USA dump; its rebuilt decompressed
`build/bm64.z64` has documented MD5 `d2f9313cccae0af85d23ac37666c0173`.
Pass this separately; `tools/prepare_sources.py` never uses retail bytes as generation
input. The MD5 is the decomp project's compatibility identifier, not a security claim.
The user supplied a retail USA dump on 2026-10-11. Its SHA-1 and runtime XXH3 both
match the expected version. Bomberhackers' decompressor at
`c7c5fd5a73a9d5969673535030fc4f1df956fa5f` produced bytes matching the exact expected
MD5 above, so a full decompilation rebuild was unnecessary for this dump. Game/RSP
and patch generation completed; all inputs/outputs remain private and ignored by Git.
Earlier ROM-free builds stopped at `Missing RecompiledFuncs/funcs.h`; that blocker
is resolved for this session. New clean checkouts still need local generation.

Required generated outputs: `RecompiledFuncs/*.c`, `funcs.h`, `recomp_overlays.inl`;
`rsp/aspMain.cpp`; `RecompiledPatches/patches.c`, `patches_bin.c`, `patches_bin.h`,
`funcs.h`, `recomp_overlays.inl`. Patches are compiled for MIPS by host clang/lld/make,
recompiled by host N64Recomp, then consumed by the Android build. Do not run NDK-built
host generators. Generated binary/C sources may contain game bytes; keep them out of Git.

## Build commands

JDK17, Android SDK35, Build Tools35.0.0, NDK28.2.13676358, CMake3.22.1,
Gradle8.11.1/AGP8.9.2. Set `ANDROID_HOME`, `JAVA_HOME`, and put SDK CMake/Ninja,
JDK bin and host clang/lld in PATH. Install host LLVM with **MIPS support** (for
example your Linux distribution's clang/lld packages): NDK28's host-running clang
only contains Android/WASM/BPF targets and cannot compile the MIPS patches.
This session built the host recompiler tools and used Debian host LLVM19.1.7
(19.1.7-3+b1), extracted into a user-owned tools directory, to build MIPS patches.
Minimum API28 (aligned_alloc dependencies).
NDK28 and 16 KiB ELF/ZIP alignment support newer Android page sizes.

```sh
git switch android-port
git submodule update --init --recursive
tools/build_host_tools.sh
python3 tools/prepare_sources.py /path/to/retail.z64 --check-only
python3 tools/prepare_sources.py /path/to/retail.z64 --decompressed /path/to/bm64/build/bm64.z64
cd android
./gradlew assembleDebug
```

For the validated big-endian retail dump, this shorter host-only decompression
path produced the exact required generation ROM. The tool remains external to the
Android application. Validate its output with `prepare_sources.py` before generation:

```sh
git clone https://github.com/Bomberhackers/bm64.git build-host-tools/bm64-decomp
git -C build-host-tools/bm64-decomp checkout c7c5fd5a73a9d5969673535030fc4f1df956fa5f
make -C build-host-tools/bm64-decomp/tools
build-host-tools/bm64-decomp/tools/bm64decompress /path/to/retail.z64 /path/to/decompressed.z64
python3 tools/prepare_sources.py /path/to/retail.z64 --decompressed /path/to/decompressed.z64
./android/gradlew -p android assembleDebug
```

ROM-free, separately identified foundation APK:

```sh
cd android
./gradlew -Pfoundation=true assembleDebug
# app/build/outputs/apk/debug/app-debug.apk
```

Verify host behavior and APK:

```sh
tools/check_android.sh
# Optional full-ROM import acceptance tests: BM64_TEST_ROM=/path/to/retail.z64 tools/check_android.sh
python3 tools/verify_apk.py android/app/build/outputs/apk/debug/app-debug.apk --sdk "$ANDROID_HOME" --foundation
# Omit --foundation for the full game APK.
```

Build renderer/runtime/frontend objects without game data (this does NOT link a game).
The optional patch step needs host LLVM with MIPS support; `PATCH_CC`/`PATCH_LD`
can select versioned compiler/linker executables. Existing patch source is GPL code
and this step does not read either ROM:

```sh
tools/prepare_patches.py
cmake -S . -B build-android-deps -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_HOME/ndk/28.2.13676358/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28 -DANDROID_STL=c++_shared \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBM64_DEPENDENCY_PROBE=ON \
  -DBM64_FILE_TO_C="$PWD/build-host-tools/file_to_c"
cmake --build build-android-deps --target BM64Recompiled PatchesLib rt64 ultramodern BM64Platform -j2
```

Android configure applies revision-checked patches idempotently and refuses conflicts.
For a desktop build after Android preparation, reverse these Android dependency patches
with `python3 tools/apply_android_patches.py --restore` before following upstream
instructions. Dependency pins stay unchanged. No force resets or dependency history edits.

Cloud networking: Gradle needs Java HTTP/HTTPS proxy properties matching the managed
HTTP proxy and the supplied CA trust store; preserve the environment proxy route.
This session used JDK17 and a local Gradle user-home configuration (not committed).

## APK download

The build-verified updated debug APK is available directly from the `android-port` branch:
<https://raw.githubusercontent.com/narmanb/BM64Recomp-Android/android-port/downloads/BM64-Android-debug.apk>.
Checksum and build notes are in `downloads/`. The ChatGPT workspace file link failed
on the user's phone; GitHub release asset uploads returned an authentication error,
so this single test APK is stored on the development branch for direct download.

## Device installation / first test

Install the full APK (`android/app/build/outputs/apk/debug/app-debug.apk`, also copied
locally to `build-apk/BM64-Android-debug.apk`). It uses a different package/storage
from the foundation build, so import the retail ROM again in **Select USA ROM**.
Tap **Start**, then **Start Game** in the retained native frontend. Touchscreen
settings/layout editor are available from the Android launcher; Android Back in
the game also exposes touch settings, including recovery from Completely Off.
If launch closes unexpectedly, reopen the launcher and **Save crash report**.

```sh
adb install -r /path/to/BM64-Android-debug.apk
```

## Known issues / next steps

1. Retest Android.5 fullscreen and settings on the S25+. User screenshots show
   frontend rendering after Android.4, but the original game title/gameplay is not
   yet confirmed. Verify bars hide/restoration, Controls → Touchscreen controls,
   percentage/mode persistence and controller independence. If graphics fails, use
   the complete ZIP export. Measure performance only once gameplay works.
2. Sideload on RP5 and S25+. Test import, Start, title screen, level entry, analog partial
   movement, all button functions, simultaneous input, menus and Bluetooth/USB reconnect.
3. Verify SRAM/EEPROM/controller-pak saves across activity termination and app restart;
   upstream runtime SaveType::AllowAll retained; quicksaving is upstream-disabled.
4. Test all visibility/opacity combinations, connect/disconnect during held inputs,
   backgrounding while moving, surface recreation, audio focus interruption, repeated
   launch/back cycles and diagnostic export after native crashes. SDL lifecycle hooks,
   input clearing and surface handoff are implemented; full simulation suspension and
   reliable GPU/audio recovery still require investigation with a running game.
5. Measure GPU features, shader startup, frame pacing and sustained FPS on stock Adreno.
   Default2x/no MSAA/60 rendering target; original game timing and patches retained.
   Tune scale/quality only against measured correctness/performance. No 60 FPS claim.
6. Launcher graphics/physical defaults apply next launch; use native settings for live
   remapping. Touch settings apply immediately in the current game/editor process.
   Foundation and game packages intentionally have separate private data/preferences.

## Verification log

- Python ROM tests: all 5 passed, including the uploaded USA ROM.
- Java touch harness: visibility policy, layout/settings round trips, opacity-independent
  input, analog partial movement, simultaneous/multiple-pointer buttons, release/cancel,
  corrupt settings clamping and safe-area geometry passed.
- Java importer harness: invalid/truncated/oversized/raw/ZIP rejection preserves previous
  file; real retail z64/v64/n64 and ZIP acceptance passed with the uploaded dump.
- Java cross-process settings harness: fresh external changes, layout retention,
  mode/opacity independence, corrupt-file recovery and temporary-file cleanup pass.
- C++ actual frontend controller defaults: face/shoulder/trigger/C/D-pad/Start/menu
  mappings and signed left-stick axes pass; menu event is mapping-independent Escape.
- C++ native pad tests: short-tap latch, clear, NaN/clamping, button OR, opposing vector
  arbitration and physical sensitivity pass. Surface-reference ownership harness passes.
  Surface-creation failure/retry harness confirms no null-surface queries, retained
  window ownership and successful retry without a second foreground event.
- Host N64Recomp/RSPRecomp at the upstream pin and file_to_c built successfully.
- Foundation Gradle assembleDebug succeeded; APK signature v2, launcher, ARM64 ABI,
  JNI exports, ELF16 KiB LOAD alignment, ZIP16 KiB alignment, fonts/controller DB and
  no ROM payload checks passed.
- ARM64 dependency/frontend probe cross-build succeeded: BM64 frontend/main objects,
  SDL2, FreeType, RmlUi, Plume, RT64, ultramodern and BM64Platform. This is an object/
  dependency check, **not a game link**. Generated game overlays/RSP/game code were
  excluded; their link/package still requires the generation inputs above.
  A later ROM-free patch generation/check compiled the original MIPS patches,
  recompiled 347 functions, embedded the patch binary, and cross-compiled both
  generated patch objects and their frontend registration for ARM64.
- Dependency patches were reversed/reapplied and checked for idempotence. Pins stay
  at the audited upstream revisions. Clean desktop dependency sources can be restored.
- Independent code review findings addressed: settings across processes, menu event
  independence, unsupported launcher menu-axis/D-pad bindings, corrupt sensitivity
  values, surface failure retries. Landscape touch settings use a scrollable dialog.
- Full game/RSP/patch generation succeeded. All 1,486 generated game C translation
  units, generated overlays and RSP, frontend/runtime, SDL and Vulkan/RT64 compiled
  for ARM64. The initial final link exposed an incorrect FindFreetype imported
  location (`freetype` as a filename); an Android `Freetype::Freetype` alias to the
  built target fixes it. The subsequent full Gradle `assembleDebug` succeeded.
- Full APK signature v2, expected package/launcher/API28/ARM64 ABI, game/preview touch
  JNI exports, lifecycle/import JNI exports, SDL_main, ELF/ZIP16 KiB alignment,
  fonts/controller database, ZIP integrity and ROM exclusion checks passed.
  `PLAYABLE=true` generated configuration verified; game dependencies resolve to
  bundled SDL2/C++ shared libraries and Android platform libraries.
- Android.2 full Gradle assembleDebug succeeded. Real-ROM import and host input/touch
  checks, production window-factory regression, dependency patch restore/apply/
  idempotence, APK v2 signature, matching update certificate, JNI/ABI/16 KiB alignment
  and ROM exclusion checks passed.
- No Android device attached. The user's S25+ Android.1 startup failed at Vulkan
  surface creation as recorded above. Android.2 survives launch but stays black
  according to the user; Android.3 diagnostics and all RP5 testing remain pending; successful title screen, gameplay, audio, saves, Vulkan/lifecycle
  recovery and FPS are **not verified**.

- Android.3 full Gradle build, APK structure/signature and unchanged signing
  certificate checks pass; diagnostic stage strings are present in libBM64.so.
  Existing ROM/input/touch tests pass. Instrumentation changes no graphics settings,
  driver selection, synchronization or simulation timing. Device log capture is
  required before identifying the black-screen root cause.

- Android.4 full native Gradle build and required APK checks succeeded. Host
  production-body regressions verify Android/desktop format consistency and queue
  selection, fatal submission/wait behavior in isolated subprocesses, full diagnostic
  ZIP content and prefix/suffix copy previews. Existing real-ROM import and input/touch
  checks pass. Independent review found no blocking issues. Final packaging is rebuilt
  fresh to avoid holes retained by AGP's incremental ZIP updater after a native library
  replacement. The fresh APK is 36,975,043 bytes; signature/ABI/JNI/16 KiB/ROM exclusion checks
  passed again on this exact package. The same update signing certificate is retained.
  Device retest pending.
