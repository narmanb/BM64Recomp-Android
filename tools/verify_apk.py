#!/usr/bin/env python3
"""Validate APK signature, ARM64 ELF/JNI payload, alignment and ROM exclusion."""
import argparse
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import zipfile


def verify(path, sdk, foundation=False):
    build_tools=sdk/'build-tools/35.0.0'
    subprocess.run([build_tools/'apksigner','verify','--verbose',path],check=True)
    subprocess.run([build_tools/'zipalign','-c','-P','16','4',path],check=True)
    badging=subprocess.check_output([build_tools/'aapt','dump','badging',path],text=True)
    expected='com.narmanb.bm64.foundation' if foundation else 'com.narmanb.bm64'
    assert f"name='{expected}'" in badging, 'wrong package'
    assert "native-code: 'arm64-v8a'" in badging, 'wrong ABI'
    assert 'com.narmanb.bm64.LauncherActivity' in badging, 'missing launcher'
    assert "sdkVersion:'28'" in badging
    with zipfile.ZipFile(path) as z:
        assert z.testzip() is None
        names=z.namelist()
        assert not any(n.lower().endswith(('.z64','.n64','.v64','.elf')) for n in names),'ROM/generated binary packaged'
        required={'lib/arm64-v8a/libBM64Platform.so','lib/arm64-v8a/libc++_shared.so'}
        if not foundation:required|={'lib/arm64-v8a/libBM64.so','lib/arm64-v8a/libSDL2.so'}
        assert required.issubset(names),'missing JNI library'
        assert 'assets/recompcontrollerdb.txt' in names,'controller DB missing'
        assert any(n.startswith('assets/assets/') and n.endswith('.ttf') for n in names),'fonts missing'
        for name in names:
            if not name.startswith('lib/'):continue
            assert name.startswith('lib/arm64-v8a/'),'unexpected ABI'
            data=z.read(name)
            assert data[:6]==b'\x7fELF\x02\x01' and struct.unpack_from('<H',data,18)[0]==183,'not AArch64 ELF64'
            phoff=struct.unpack_from('<Q',data,32)[0]
            size,count=struct.unpack_from('<HH',data,54)
            for i in range(count):
                off=phoff+i*size
                if struct.unpack_from('<I',data,off)[0]==1:
                    assert struct.unpack_from('<Q',data,off+48)[0]>=16384,f'{name}: ELF segment not 16 KiB aligned'
        bridge=z.read('lib/arm64-v8a/libBM64Platform.so')
        # These names must be exported in the dynamic symbol table, not just present in source.
        with tempfile.TemporaryDirectory() as temp:
            lib=Path(temp)/'bridge.so';lib.write_bytes(bridge)
            symbols=subprocess.check_output(['readelf','--dyn-syms','--wide',lib],text=True)
            for function in ('nativeSetState','nativeClearState','nativePollPreview','nativeIsMenuOpen','nativeRequestMenuToggle'):
                assert 'Java_com_narmanb_bm64_touch_NativeTouch_'+function in symbols,'missing JNI '+function
            if not foundation:
                game_library=z.read('lib/arm64-v8a/libBM64.so')
                # Native UI calls this Java activity method through GetMethodID.
                # Search all dex files: multidex may place the activity in classes2.dex.
                assert b'showTouchscreenSettings' in game_library, 'missing native touch settings callback'
                assert any(b'showTouchscreenSettings' in z.read(n) for n in names if n.endswith('.dex')), 'missing Java touch settings callback'
                lib.write_bytes(game_library)
                symbols=subprocess.check_output(['readelf','--dyn-syms','--wide',lib],text=True)
                for function in ('SDL_main','Java_com_narmanb_bm64_MainActivity_nativeInit','Java_com_narmanb_bm64_MainActivity_nativeDocumentResult','Java_com_narmanb_bm64_MainActivity_nativeSetForeground','Java_com_narmanb_bm64_MainActivity_nativeControllerSensitivity'):
                    assert function in symbols,'missing game entry '+function
                # The SDL game process loads BM64, while the launcher preview loads
                # BM64Platform. Both libraries must implement the touch JNI contract.
                for function in ('nativeSetState','nativeClearState','nativePollPreview','nativeIsMenuOpen','nativeRequestMenuToggle'):
                    assert 'Java_com_narmanb_bm64_touch_NativeTouch_'+function in symbols,'missing game touch JNI '+function
    print('APK checks passed; this does not verify device behavior or gameplay')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('apk',type=Path);p.add_argument('--sdk',type=Path,default=Path(os.environ.get('ANDROID_HOME','/nonexistent')));p.add_argument('--foundation',action='store_true')
    a=p.parse_args();verify(a.apk,a.sdk,a.foundation)
