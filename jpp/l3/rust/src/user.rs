use crate::{dhsetup::DHSetup, ring::Ring};
use rand::{RngExt};

pub struct User<const P: u64> {
    secret: u64,
    public_key: u64,
    secret_key: u64,
}

impl <const P: u64> User<P> {
    pub fn new(dh: &DHSetup<P>) -> Self {
        let secret = rand::rng().random_range(1..P);
        let generator = dh.get_generator();
        let public_key = Ring::<P>::new(generator).pow(secret).get_value().unwrap();
        Self { secret, public_key, secret_key: 0 }
    }

    pub fn set_key(&mut self, other_public_key: u64) {
        self.secret_key = Ring::<P>::new(other_public_key).pow(self.secret).get_value().unwrap();
    }

    pub fn get_public_key(&self) -> u64 {
        self.public_key
    }

    pub fn encrypt(&self, msg: u64) -> u64 {
        (Ring::<P>::new(msg) * Ring::<P>::new(self.secret_key)).get_value().unwrap()
    }

    pub fn decrypt(&self, cipher: u64) -> u64 {
        (Ring::<P>::new(cipher) / Ring::<P>::new(self.secret_key)).get_value().unwrap()
    }
}