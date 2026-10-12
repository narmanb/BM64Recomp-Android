#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
python3 tools/apply_android_patches.py
python3 -m unittest discover -s tests -v
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT
"${CXX:-c++}" -std=c++20 -pthread -Iinclude tests/test_native_pad.cpp -o "$build/pad"
"$build/pad"
"${CXX:-c++}" -std=c++20 -Iinclude tests/test_window_handoff.cpp -o "$build/handoff"
"$build/handoff"
"${CXX:-c++}" -std=c++20 -Ilib/rt64/src/contrib/plume tests/test_android_surface.cpp -o "$build/surface"
"$build/surface"
sdl_headers=build-android-deps/_deps/sdl2-src/include
if [[ ! -f "$sdl_headers/SDL.h" ]]; then
    sdl_headers=$(find android/app/.cxx -path '*/_deps/sdl2-src/include/SDL.h' -print -quit 2>/dev/null || true)
    sdl_headers=$(dirname "${sdl_headers:-/missing/SDL.h}")
fi
if [[ -f "$sdl_headers/SDL.h" ]]; then
    python3 tests/extract_renderer_tests.py "$build"
    "${CXX:-c++}" -std=c++20 -D__ANDROID__ -I"$build" -Ilib/rt64/src/contrib/plume \
        -Ilib/rt64/src/contrib/plume/contrib/Vulkan-Headers/include tests/test_vulkan_failure.cpp -o "$build/vulkan-failure"
    "$build/vulkan-failure"
    for platform in android desktop; do
        platform_flags=(-DPLUME_SDL_VULKAN_ENABLED)
        [[ "$platform" == android ]] && platform_flags=(-D__ANDROID__)
        for test in test_render_formats test_android_queues test_msaa_formats test_vulkan_sample_counts test_color_precision; do
            "${CXX:-c++}" -std=c++20 "${platform_flags[@]}" -I"$build" -I"$sdl_headers" \
                -Ilib/rt64/src/contrib/plume -Ilib/rt64/src/contrib/plume/contrib/Vulkan-Headers/include \
                "tests/$test.cpp" -o "$build/$test-$platform"
            "$build/$test-$platform"
        done
    done
    python3 - "$build/window_factory.inc" <<'WINDOW_CODE'
from pathlib import Path
import sys
source=Path('src/main/main.cpp').read_text()
start=source.index('ultramodern::renderer::WindowHandle create_window(')
end=source.index('\n}\n',start)+3
Path(sys.argv[1]).write_text(source[start:end])
WINDOW_CODE
    "${CXX:-c++}" -std=c++20 -D__ANDROID__ -I"$build" -I"$sdl_headers" tests/test_android_window.cpp -o "$build/window"
    "$build/window"
    "${CXX:-c++}" -std=c++20 -Iinclude -I"$sdl_headers" tests/test_android_menu.cpp -o "$build/menu"
    "$build/menu"
    python3 - "$build/controller_defaults.inc" <<'PYCODE'
from pathlib import Path
import sys
source=Path('src/game/input.cpp').read_text()
start=source.index('enum class InputType')
end=source.index('};',start)+2
out=source[start:end]+"\n"
start=source.index('constexpr SDL_GameControllerButton SDL_CONTROLLER_BUTTON_SOUTH')
end=source.index('const recomp::DefaultN64Mappings',start)
out+=source[start:end]+"\n"
start=source.index('const recomp::DefaultN64Mappings recomp::default_n64_controller_mappings')
end=source.index('};',start)+2
Path(sys.argv[1]).write_text(out+source[start:end])
PYCODE
    "${CXX:-c++}" -std=c++20 -Iinclude -I"$build" -I"$sdl_headers" \
        -Ilib/N64ModernRuntime/ultramodern/include -Ilib/N64ModernRuntime/thirdparty \
        tests/test_controller_mappings.cpp -o "$build/mappings"
    "$build/mappings"
else
    echo "Menu event test skipped: run the Android dependency configure to fetch pinned SDL headers."
fi
python3 - "$build/SDLWindowStyleHarness.java" <<'SDL_JAVA'
from pathlib import Path
import sys
source=Path('android/app/src/main/java/org/libsdl/app/SDLActivity.java').read_text()
start=source.index('    boolean sendCommand(int command, Object data)')
end=source.index('\n    // C functions we call',start)
template=Path('tests/SDLWindowStyleHarness.java.in').read_text()
Path(sys.argv[1]).write_text(template.replace('    /* ACTUAL_SEND_COMMAND */',source[start:end]))
SDL_JAVA
javac -d "$build/classes" "$build/SDLWindowStyleHarness.java"
java -cp "$build/classes" SDLWindowStyleHarness
java_src=android/app/src/main/java/com/narmanb/bm64
javac -d "$build/classes" "$java_src"/touch/{TouchControl,TouchLayout,TouchPad,TouchVisibility,TouchStore}.java "$java_src/RomImporter.java" "$java_src/DiagnosticFiles.java" tests/{TouchHarness,TouchStoreHarness,RomImporterHarness,DiagnosticFilesHarness}.java
java -cp "$build/classes" com.narmanb.bm64.DiagnosticFilesHarness
java -cp "$build/classes" TouchHarness
java -cp "$build/classes" TouchStoreHarness
if [[ -n "${BM64_TEST_ROM:-}" ]]; then
    java -cp "$build/classes" RomImporterHarness "$BM64_TEST_ROM"
else
    java -cp "$build/classes" RomImporterHarness
fi
