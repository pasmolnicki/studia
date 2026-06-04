use rand::RngExt;
use crate::utils::{inverse, mod_pow};

pub struct RSA<const P: u64, const Q: u64> {
    e: u64,
    d: u64,
    n: u64,
}

const fn gcd(mut a: u64, mut b: u64) -> u64 {
    while b != 0 {
        let temp = b;
        b = a % b;
        a = temp;
    }
    a
}

fn find_exponent(lcm: u64) -> Option<u64> {
    // use random number generator to find e such that 1 < e < lcm and gcd(e, lcm) = 1
    const MAX_ITER: u64 = 10000;
    let mut rng = rand::rng();

    for _ in 0..MAX_ITER {
        let e = rng.random_range(2..lcm);
        if gcd(e, lcm) == 1 {
            return Some(e);
        }
    }
    None
}

impl <const P: u64, const Q: u64> RSA<P, Q> {

    pub fn new() -> Self {
        let n = P * Q;
        let lcm = (P - 1) * (Q - 1) / gcd(P - 1, Q - 1);
        let e = find_exponent(lcm)
            .expect("Failed to find exponent e");
        let d = inverse(e, lcm)
            .expect(&format!("Couldn't find private key for exp={e}"));

        // generic `Self` types are currently not permitted in anonymous constants
        // const LCM: u64 = (P - 1) * (Q - 1) / gcd(P - 1, Q - 1);
        // let d = Ring::<{Self::LCM}>::new(e).inverse()
            // .expect(&format!("Couldn't find private key for exp={e}"));
    
        Self { e, d, n }
    }

    pub fn public_key(&self) -> u64 {
        self.e
    }

    pub fn encrypt(&self, msg: u64, public_key: u64) -> u64 {
        mod_pow(msg, public_key, self.n)
    }

    pub fn decrypt(&self, cipher: u64) -> u64 {
        mod_pow(cipher, self.d, self.n)
    }

    pub fn getModulo() -> u64 {
        P * Q
    }
}