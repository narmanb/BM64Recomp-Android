import hashlib
import importlib.util
import os
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('prepare', ROOT/'tools/prepare_sources.py')
prepare = importlib.util.module_from_spec(spec)
spec.loader.exec_module(prepare)

class RomTests(unittest.TestCase):
    def test_byte_orders(self):
        raw = bytes.fromhex('80371240aabbccdd')
        self.assertEqual(prepare.normalize_rom(raw), raw)
        self.assertEqual(prepare.normalize_rom(bytes.fromhex('37804012bbaaddcc')), raw)
        self.assertEqual(prepare.normalize_rom(bytes.fromhex('40123780ddccbbaa')), raw)
    def test_reject_not_rom(self):
        for data in (b'', b'PK12', b'junk', bytes.fromhex('80371240ff')):
            with self.assertRaises(ValueError): prepare.normalize_rom(data)
    def test_reject_wrong_game(self):
        with self.assertRaisesRegex(ValueError, '8 MiB'): prepare.validate_rom(bytes.fromhex('80371240'))
        data = bytes.fromhex('80371240') + bytes(8*1024*1024-4)
        with self.assertRaisesRegex(ValueError, 'USA'): prepare.validate_rom(data)
    def test_retail_not_generation_input(self):
        data = bytes.fromhex('80371240') + bytes(8*1024*1024-4)
        with self.assertRaisesRegex(ValueError, 'decompressed'): prepare.validate_decompressed(data)
    @unittest.skipUnless(os.environ.get('BM64_TEST_ROM'), 'User ROM not provided')
    def test_user_rom(self):
        data = prepare.validate_rom(Path(os.environ['BM64_TEST_ROM']).read_bytes())
        self.assertEqual(hashlib.sha1(data).hexdigest(), prepare.ROM_SHA1)

if __name__ == '__main__': unittest.main()
