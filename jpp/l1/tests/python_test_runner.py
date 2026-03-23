#!/usr/bin/env python3
"""Build shared libraries and run Python ctypes wrappers tests."""
import subprocess
import sys
import os
# Ensure tests package path is importable when running from repo root
sys.path.insert(0, os.path.join(os.getcwd(), 'tests'))
import c_wrapper, rust_wrapper, ada_wrapper

def run(cmd):
    print('=>', cmd)
    p = subprocess.run(cmd, shell=True)
    if p.returncode != 0:
        raise SystemExit(f"Command failed: {cmd}")

def build_shared():
    run('make build-shared')

def validate_wrappers():
    print('Testing C wrapper...')
    assert c_wrapper.gcd(48,18) == 6
    assert c_wrapper.min_divider(15) == 3
    assert c_wrapper.totient(10) == 4
    assert c_wrapper.diofant(48,18,6) == (2,5)

    print('Testing Rust wrapper...')
    assert rust_wrapper.gcd(48,18) == 6
    assert rust_wrapper.min_divider(15) == 3
    assert rust_wrapper.totient(10) == 4
    assert rust_wrapper.diofant(48,18,6) == (2,5)

    print('Testing Ada wrapper...')
    assert ada_wrapper.gcd(48,18) == 6
    assert ada_wrapper.min_divider(15) == 3
    assert ada_wrapper.totient(10) == 4
    assert ada_wrapper.diofant(48,18,6) == (2,5)

    print('\nAll wrapper tests passed.')

def main():
    build_shared()
    validate_wrappers()

if __name__ == '__main__':
    main()
