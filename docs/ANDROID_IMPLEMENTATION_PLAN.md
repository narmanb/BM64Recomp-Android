# Android foundation implementation plan

Execution: inline, as requested. Spec: ANDROID_DESIGN.md.

1. ROM and build preparation (`tools/prepare_sources.py`, host bootstrap, tests):
   verify USA retail SHA-1; normalize z64/v64/n64; separately require decompressed
   input; fail before mutations when host tools/submodules are missing; consume only
   generated C/RSP/patch artifacts in Android. Verify negative and byte-order cases.
2. Launcher and Android packaging (`android/`, `src/main/`, `CMakeLists.txt`):
   pin SDK35/NDK28.2/Gradle8.11.1/AGP8.9.2/SDL2.32.8, API28 and ARM64;
   adapt Hero SAF/diagnostics/private storage. Add explicitly non-playable foundation
   variant. Retain desktop target and native game generation gates. Build debug APK.
3. Controls (`android/.../touch`, native input state, `src/game/controls.cpp`):
   adapt Goemon v1.0.8 overlay/editor with N64 mapping and configurable touch response;
   persist visibility/opacity/layout; connection callbacks; launcher recovery settings;
   preserve SDL physical remapping and menus; test multitouch/cancel/visibility/latch.
4. Renderer (`android/patches`, `tools/apply_android_patches.py`):
   preserve current BM64 RT64 revision, adapt Goemon Android build/window/Adreno changes,
   private data path with detectDataPath=false; build dependencies without ROM where
   possible; document incomplete GPU/surface validation.
5. Verify, review, commit and push only android-port. Update ANDROID_PORT.md with
   commands/results, missing generated inputs and device test checklist.

Review focus: an already connected controller; disconnect while touch is held;
corrupt preferences; invalid import preserving an existing ROM; focus loss with held
inputs; opposite physical/touch stick directions; desktop behavior isolation; Android
16 KiB page-size packaging. Device work remains gated on ROM-generated native game.
