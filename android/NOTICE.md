# Android source provenance

BM64Recomp-Android inherits BM64Recomp's GPL-3.0 license (root COPYING).
No copyrighted ROMs or generated game assets are included.

- TouchControl, TouchLayout, TouchPad, TouchPrefs, TouchOverlayView and
  TouchOverlayController adapted from ogdanimal/Goemon64Recomp-Android tag v1.0.8,
  commit 367b1017907d08867d70a4b2e9730b21d75577ff, GPL-3.0 (COPYING).
  N64 mapping, connection visibility, opacity bounds, response and persistence differ.
- LauncherActivity, MainActivity, DiagnosticFiles, RomImporter and Android glue
  adapted from narmanb/BMHeroRecomp-android android-port, commit recorded in
  docs/ANDROID_PORT.md, GPL-3.0 (COPYING).
- org/libsdl/app sources are SDL 2.32.8, zlib license; see `app/src/main/java/org/libsdl/LICENSE.txt`.
  Native SDL must match this version. Gradle wrapper originates in the reference
  ports, Apache-2.0; distribution is pinned and SHA-256 checked.
- RT64/Plume Android changes are adapted from ogdanimal/rt64 and renderbag/plume
  pinned revisions, with retained MIT headers/licenses in those dependencies.
  Replayable Android patches keep BM64's existing dependency pins and game fixes.

Settings/input tests and ARM64 foundation bridge are independently buildable without
ROM generation. The foundation APK is not a playable game.
