#!/usr/bin/env python3
"""Validate retail ROM separately from upstream's decompressed generation ROM."""
import argparse
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
ROM_SHA1 = '8a7648d8105ac4fc1ad942291b2ef89aeca921c9'
DECOMP_MD5 = 'd2f9313cccae0af85d23ac37666c0173'


def normalize_rom(data):
    if len(data) < 4 or len(data) % 4:
        raise ValueError('ROM size must be a nonzero multiple of four bytes')
    if data[:4] == bytes.fromhex('80371240'):
        return data
    out = bytearray(len(data))
    if data[:4] == bytes.fromhex('37804012'):
        out[0::2], out[1::2] = data[1::2], data[0::2]
    elif data[:4] == bytes.fromhex('40123780'):
        for i in range(4): out[i::4] = data[3-i::4]
    else:
        raise ValueError('Not an N64 ROM (.z64, .v64 or .n64)')
    return bytes(out)


def validate_rom(data):
    if len(data) != 8*1024*1024:
        raise ValueError('Expected 8 MiB Bomberman 64 USA retail ROM')
    result = normalize_rom(data)
    if hashlib.sha1(result).hexdigest() != ROM_SHA1:
        raise ValueError(f'Expected Bomberman 64 USA SHA-1 {ROM_SHA1}')
    return result


def validate_decompressed(data):
    result = normalize_rom(data)
    if hashlib.md5(result).hexdigest() != DECOMP_MD5:
        raise ValueError(f'Expected rebuilt decompressed USA ROM (MD5 {DECOMP_MD5}); '
                         'follow upstream BUILDING.md/Bomberhackers/bm64, not the retail dump')
    return result


def run(command):
    subprocess.run([str(x) for x in command], cwd=ROOT, check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom', type=Path, help='User retail USA dump (runtime input)')
    parser.add_argument('--decompressed', type=Path, help='Rebuilt decompressed USA ROM (generation input)')
    parser.add_argument('--check-only', action='store_true')
    args = parser.parse_args()
    if args.rom.stat().st_size != 8*1024*1024:
        raise ValueError('Expected 8 MiB USA retail dump')
    validate_rom(args.rom.read_bytes())
    decomp = None
    if args.decompressed:
        if not 8*1024*1024 < args.decompressed.stat().st_size <= 64*1024*1024:
            raise ValueError('Expected decompressed generation ROM larger than retail, at most 64 MiB')
        decomp = validate_decompressed(args.decompressed.read_bytes())
    if args.check_only:
        print('Retail USA ROM verified' + ('; decompressed generation ROM verified' if decomp else ''))
        return
    if decomp is None:
        raise ValueError('Generation needs --decompressed /path/to/build/bm64.z64. Retail ROM is only for runtime.')
    for tool in ('N64Recomp', 'RSPRecomp', 'build-host-tools/file_to_c'):
        if not os.access(ROOT/tool, os.X_OK): raise ValueError(f'Missing executable host tool: {tool}; run tools/build_host_tools.sh')
    for tool in ('clang', 'ld.lld', 'make'):
        if not shutil.which(tool): raise ValueError(f'Missing host tool: {tool}')
    if not (ROOT/'BM64Syms/dump.toml').is_file(): raise ValueError('Initialize recursive submodules')
    # Never copy the retail ROM to generation input, nor package either ROM.
    dest = ROOT/'bm64.decomp.us.z64'
    tmp = dest.with_suffix('.z64.tmp')
    try:
        tmp.write_bytes(decomp)
        tmp.replace(dest)
    finally: tmp.unlink(missing_ok=True)
    (ROOT/'rsp').mkdir(exist_ok=True)
    (ROOT/'RecompiledPatches').mkdir(exist_ok=True)
    run([ROOT/'N64Recomp', 'bm64.us.toml'])
    run([ROOT/'RSPRecomp', 'aspMain.toml'])
    run(['make', '-C', 'patches', 'CC=clang', 'LD=ld.lld'])
    run([ROOT/'N64Recomp', 'patches.toml'])
    run([ROOT/'build-host-tools/file_to_c', 'patches/patches.bin', 'mm_patches_bin',
         'RecompiledPatches/patches_bin.c', 'RecompiledPatches/patches_bin.h'])
    print('Generated game/RSP/patch sources; not an APK build')


if __name__ == '__main__':
    try: main()
    except (OSError, ValueError, subprocess.CalledProcessError) as e:
        print(f'Source preparation failed: {e}', file=sys.stderr)
        sys.exit(1)
