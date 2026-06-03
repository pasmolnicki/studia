pub fn inverse(a: u64, n: u64) -> Option<u64> {
    let mut t: isize = 0;
    let mut new_t: isize = 1;
    let mut r: isize = n as isize;
    let mut new_r: isize = (a % n) as isize;

    while new_r != 0 {
        let q = r / new_r;

        let temp_t = t - q * new_t;
        t = new_t;
        new_t = temp_t;

        let temp_r = r - q * new_r;
        r = new_r;
        new_r = temp_r;
    }

    if r > 1 {
        return None;
    }

    if t < 0 {
        t += n as isize;
    }

    Some(t as u64)
}

pub fn mod_pow(mut base: u64, mut exp: u64, modulus: u64) -> u64 {
    if modulus == 1 { return 0; }

    let mut result: u128 = 1;
    base %= modulus as u64;
    let modulus_128 = modulus as u128;

    while exp > 0 {
        if exp % 2 == 1 {
            result = (result * (base as u128)) % modulus_128;
        }
        exp >>= 1;
        base = ((base as u128 * base as u128) % modulus_128) as u64;
    }
    result as u64
}