#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
revision=98bf104b1b5ed83126af8bcab0cc964782617dbf
if [[ ! -d build-host-tools/N64Recomp/.git ]]; then
    git clone https://github.com/N64Recomp/N64Recomp.git build-host-tools/N64Recomp
fi
git -C build-host-tools/N64Recomp checkout "$revision"
git -C build-host-tools/N64Recomp submodule update --init --recursive
cmake -S build-host-tools/N64Recomp -B build-host-tools/recompiler -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-host-tools/recompiler --target N64RecompCLI RSPRecomp -j "${BUILD_JOBS:-2}"
cp build-host-tools/recompiler/N64Recomp build-host-tools/recompiler/RSPRecomp .
"${CXX:-c++}" -std=c++17 -O2 lib/rt64/src/tools/file_to_c/file_to_c.cpp -o build-host-tools/file_to_c
