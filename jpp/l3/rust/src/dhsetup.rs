use rand::RngExt;

use crate::utils::mod_pow;

pub struct DHSetup<const P: u64> {
    generator: u64,
}

impl <const P: u64> DHSetup<P> {
    pub fn new() -> Self {
        DHSetup { generator: Self::find_generator() }
    }

    pub fn get_generator(&self) -> u64 {
        self.generator
    }

    fn factorize(mut n: u64) -> Vec<u64> {
        let mut factors = Vec::new();
        let mut d = 2;
        let mut first_division = true;

        while n > 1 {
            if n % d == 0 {
                if first_division {
                    factors.push(d);
                    first_division = false;
                }
                n /= d;
            } else {
                d += 1;
                first_division = true;
            }
        }
        
        factors
    }

    fn is_generator(g: u64, factors: &[u64]) -> bool {
        for &factor in factors {
            if mod_pow(g, (P - 1) / factor, P) == 1 {
                return false;
            }
        }
        true
    }

    fn find_generator() -> u64 {
        let factors = Self::factorize(P - 1);
        let mut rng = rand::rng();
        let mut g;

        loop {
            g = rng.random_range(2..P);
            if Self::is_generator(g, &factors) {
                break;
            }
        }
        g
    }
}