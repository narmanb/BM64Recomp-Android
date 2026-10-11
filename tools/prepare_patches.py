#!/usr/bin/env python3
"""Compile and recompile the existing GPL game patches without a ROM."""
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[1]

def check_requirements():
    compiler=os.environ.get('PATCH_CC','clang')
    linker=os.environ.get('PATCH_LD','ld.lld')
    for tool in (compiler,linker,'make'):
        if not shutil.which(tool):raise ValueError(f'Missing host tool {tool}; install host LLVM with MIPS support')
    targets=subprocess.check_output([compiler,'--print-targets'],text=True)
    if not any(line.strip().startswith('mips ') for line in targets.splitlines()):
        raise ValueError('Host patch clang needs MIPS support; the Android NDK clang is insufficient')
    for tool in ('N64Recomp','build-host-tools/file_to_c'):
        if not os.access(ROOT/tool,os.X_OK):raise ValueError(f'Missing executable host tool {tool}; run tools/build_host_tools.sh')
    if not (ROOT/'BM64Syms/dump.toml').is_file():raise ValueError('Initialize recursive submodules')
    return compiler,linker

def prepare():
    compiler,linker=check_requirements()
    def run(command):subprocess.run([str(x) for x in command],cwd=ROOT,check=True)
    (ROOT/'RecompiledPatches').mkdir(exist_ok=True)
    run(['make','-C','patches',f'CC={compiler}',f'LD={linker}'])
    run([ROOT/'N64Recomp','patches.toml'])
    run([ROOT/'build-host-tools/file_to_c','patches/patches.bin','mm_patches_bin',
         'RecompiledPatches/patches_bin.c','RecompiledPatches/patches_bin.h'])
    print('Generated ROM-free patches only; game and RSP generation still require the decompressed ROM')

if __name__=='__main__':
    try:prepare()
    except (OSError,ValueError,subprocess.CalledProcessError) as e:
        print(f'Patch preparation failed: {e}',file=sys.stderr);sys.exit(1)
