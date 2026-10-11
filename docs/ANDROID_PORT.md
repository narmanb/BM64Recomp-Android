# Bomberman 64 Android port — development status

Branch: `android-port` in **narmanb/BM64Recomp-Android**. Started from `master`
`726adc908ca7abd27c4cba12d1f52a0c243727df`, identical to RevoSucks/BM64Recomp
master at audit. Upstream and master are not modified. This is static recompilation,
not an emulator. No ROM, extracted assets or generated game binary is committed.

## Current status

The **full native ARM64 debug APK now builds** directly in this cloud environment
(2026-10-11). Package `com.narmanb.bm64`, version `1.0.0-android.1`, minimum API28.
The game/RSP/patch code is generated and linked into `libBM64.so`; Vulkan/RT64,
SDL2 and the Android launcher/control system are packaged. Start is enabled after
importing a compatible retail ROM. The ROM is **not bundled**.

APK SHA-256: `75649b7f1ade839c2ced6d9ab590b40f07677d943525031bc002ff1fa56a734d`.
This is the first full build for device testing, **not verified playable/stable**.
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

**No title screen, gameplay, audio, saves, Vulkan rendering, lifecycle recovery
or performance result has been verified on a physical device.**

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

1. Run the first physical-device test; build/package validation is complete but
   runtime initialization and gameplay remain unverified.
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
- No Android device attached (`adb devices` empty); RP5/S25+ title screen, gameplay,
  audio, saves, touch/physical behavior, Vulkan/lifecycle and FPS remain **not tested**.
