use std::ops::{Add, Mul, Sub, Div};

pub type IntegerType = isize;

// T - [i32, f64, ...]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct GaussianType
{
    real: IntegerType,
    imag: IntegerType,
}

pub fn gcd(a: GaussianType, b: GaussianType) -> GaussianType
{
    let mut a = a;
    let mut b = b;

    while b.real != 0 || b.imag != 0 {
        let r = a.div_mod(b).1;
        a = b;
        b = r;
    }
    a
}

pub fn lcm(a: GaussianType, b: GaussianType) -> GaussianType
{
    (a * b) / gcd(a, b)
}


impl GaussianType
{
    pub fn new(real: IntegerType, imag: IntegerType) -> Self {
        Self { real, imag }
    }

    // N(z) = a^2 + b^2
    pub fn norm(&self) -> IntegerType {
        self.real * self.real + self.imag * self.imag
    }

    pub fn conjugate(&self) -> Self {
        Self {
            real: self.real,
            imag: -self.imag,
        }
    }

    pub fn div_mod(self, other: Self) -> (Self, Self) {
        let quotient = self / other;
        let remainder = self - quotient * other;
        (quotient, remainder)
    }
}

impl Add for GaussianType
{
    type Output = Self;

    fn add(self, other: Self) -> Self {
        Self {
            real: self.real + other.real,
            imag: self.imag + other.imag,
        }
    }
}

impl Sub for GaussianType
{
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        Self {
            real: self.real - other.real,
            imag: self.imag - other.imag,
        }
    }
}

impl Mul for GaussianType
{
    type Output = Self;

    // (a + bi)(c + di) = (ac - bd) + (ad + bc)i
    fn mul(self, other: Self) -> Self {
        Self {
            real: self.real * other.real - self.imag * other.imag,
            imag: self.real * other.imag + self.imag * other.real,
        }
    }
}

impl Div for GaussianType
{
    type Output = Self;

    // (a + bi) / (c + di) = [(a + bi)(c - di)] / (c^2 - (di)^2) = [(ac + bd) + (bc - ad)i] / (c^2 + d^2)
    fn div(self, other: Self) -> Self {
        let denominator = other.norm();
        let numerator = self * other.conjugate();

        Self {
            // rounding to nearest integer
            real: (numerator.real as f64 / denominator as f64).round() as IntegerType,
            imag: (numerator.imag as f64 / denominator as f64).round() as IntegerType,
        }
    }
}

impl std::fmt::Display for GaussianType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} + {}i", self.real, self.imag)
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_add() {
        let a = GaussianType::new(1, 2);
        let b = GaussianType::new(3, 4);
        assert_eq!(a + b, GaussianType::new(4, 6));
    }

    #[test]
    fn test_sub() {
        let a = GaussianType::new(5, 6);
        let b = GaussianType::new(2, 3);
        assert_eq!(a - b, GaussianType::new(3, 3));
    }

    #[test]
    fn test_mul() {
        let a = GaussianType::new(1, 2);
        let b = GaussianType::new(3, 4);
        assert_eq!(a * b, GaussianType::new(-5, 10));
    }

    #[test]
    fn test_div() {
        let a = GaussianType::new(5, 1);
        let b = GaussianType::new(2, 1);
        assert_eq!(a / b, GaussianType::new(2, -1));
    }

    #[test]
    fn test_gcd() {
        let a = GaussianType::new(5, 1);
        let b = GaussianType::new(2, 1);
        assert_eq!(gcd(a, b), GaussianType::new(2, 1));
    }

    #[test]
    fn test_lcm() {
        let a = GaussianType::new(5, 1);
        let b = GaussianType::new(2, 1);
        assert_eq!(lcm(a, b), GaussianType::new(5, 1));
    }
}