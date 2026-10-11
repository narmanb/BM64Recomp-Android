#!/usr/bin/env python3
"""Replay Android dependency adaptations at BM64's unchanged desktop revisions."""
from pathlib import Path
import subprocess
import sys
import argparse
ROOT=Path(__file__).resolve().parents[1]
PATCHES=(('lib/rt64','rt64-android.patch','71b5906f0b991459744dabfcc68d04616ac71797'),
         ('lib/rt64/src/contrib/plume','plume-android.patch','51b1ad443b9f202c5cfc930ae25345d3f2ba7716'))
def apply(restore=False):
    for path, name, revision in PATCHES:
        repo=ROOT/path;patch=ROOT/'android/patches'/name
        if subprocess.check_output(['git','-C',str(repo),'rev-parse','HEAD'],text=True).strip()!=revision:
            raise ValueError(f'{path}: revision drift; review {name} first')
        def check(reverse=False):
            return subprocess.run(['git','-C',str(repo),'apply','--check',*(['--reverse'] if reverse else []),str(patch)],capture_output=True).returncode==0
        already=check(True)
        if restore:
            if not already:
                if check(): print(f'{path}: Android patch already absent');continue
                raise ValueError(f'{path}: conflicting edits; refusing restoration')
            subprocess.run(['git','-C',str(repo),'apply','--reverse',str(patch)],check=True)
            print(f'{path}: Android patch restored to desktop sources')
        else:
            if already: print(f'{path}: Android patch already applied');continue
            if not check(): raise ValueError(f'{path}: conflicting edits; Android patch not applied')
            subprocess.run(['git','-C',str(repo),'apply',str(patch)],check=True)
            print(f'{path}: Android patch applied')
if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore',action='store_true',help='Reverse only the known Android patches for desktop builds')
    args=parser.parse_args()
    try: apply(args.restore)
    except (OSError,ValueError,subprocess.CalledProcessError) as e:
        print(e,file=sys.stderr);sys.exit(1)
