#!/usr/bin/env python3
import c_wrapper, rust_wrapper, ada_wrapper

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
    validate_wrappers()

if __name__ == '__main__':
    main()
