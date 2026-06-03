#[allow(dead_code)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Ring <const N: u64> {
    value: Option<u64>,
}

impl <const N: u64> Ring<N> {
    #[allow(dead_code)]
    pub fn default() -> Self {
        Ring { value: Some(0) }
    }

    pub fn new(value: u64) -> Self {
        Ring { value: Some(value % N) }
    }

    pub fn get_value(&self) -> &Option<u64> {
        &self.value
    }

    pub fn pow(&self, exp: u64) -> Self {
        if self.value.is_none() { return Ring { value: None }; }
        Ring::new(crate::utils::mod_pow(self.value.unwrap(), exp, N))
    }

    pub fn inverse(&self) -> Option<u64> {
        let a = self.value.unwrap();

        let mut t: isize = 0;
        let mut new_t: isize = 1;
        let mut r: isize = N as isize;
        let mut new_r: isize = (a % N) as isize;

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
            t += N as isize;
        }

        Some(t as u64)
    }
}

// +, -, *, /
impl <const N: u64> std::ops::Add for Ring<N> {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        if self.value.is_none() || other.value.is_none() { return Ring { value: None }; }
        Ring::new((self.value.unwrap() + other.value.unwrap()) % N)
    }
}

impl <const N: u64> std::ops::Mul for Ring<N> {
    type Output = Self;

    fn mul(self, other: Self) -> Self {
        if self.value.is_none() || other.value.is_none() { return Ring { value: None }; }
        Ring::new((self.value.unwrap() * other.value.unwrap()) % N)
    }
}

impl <const N: u64> std::ops::Sub for Ring<N> {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        if self.value.is_none() || other.value.is_none() { return Ring { value: None }; }
        Ring::new((self.value.unwrap() + N - other.value.unwrap()) % N)
    }
}

impl <const N: u64> std::ops::Div for Ring<N> {
    type Output = Self;

    fn div(self, other: Self) -> Self {
        let inverse = other.inverse();
        if self.value.is_none() || inverse.is_none() { return Ring { value: None }; }
        Ring::new((self.value.unwrap() * inverse.unwrap()) % N)
    }
}

// +=, *=, -=, /=
impl <const N: u64> std::ops::AddAssign for Ring<N> {
    fn add_assign(&mut self, other: Self) {
        self.value = (*self + other).get_value().clone();
    }
}

impl <const N: u64> std::ops::MulAssign for Ring<N> {
    fn mul_assign(&mut self, other: Self) {
        self.value = (*self * other).get_value().clone();
    }
}

impl <const N: u64> std::ops::SubAssign for Ring<N> {
    fn sub_assign(&mut self, other: Self) {
        self.value = (*self - other).get_value().clone();
    }
}

impl <const N: u64> std::ops::DivAssign for Ring<N> {
    fn div_assign(&mut self, other: Self) {
        *self = *self / other;
    }
}

// Negation
impl <const N: u64> std::ops::Neg for Ring<N> {
    type Output = Self;
    fn neg(self) -> Self {
        if self.value.is_none() { return Ring { value: None }; }
        Ring::new((N - self.value.unwrap()) % N)
    }
}

// Formatting
impl <const N: u64> std::fmt::Display for Ring<N> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self.value {
            Some(v) => write!(f, "{}", v),
            None => write!(f, "None"),
        }
    }
}

impl <const N: u64> From<u64> for Ring<N> {
    fn from(value: u64) -> Self {
        Self{value: Some(value)}
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_constructors() {
        let r0: Ring<5> = Ring::default();
        assert_eq!(r0.get_value(), &Some(0));

        let r1: Ring<5> = Ring::new(3);
        assert_eq!(r1.get_value(), &Some(3));

        let r2: Ring<5> = Ring::new(8);
        assert_eq!(r2.get_value(), &Some(3));
    }

    #[test]
    fn test_addition() {
        let r1: Ring<5> = Ring::new(3);
        let r2: Ring<5> = Ring::new(4);
        let r3 = r1 + r2;
        assert_eq!(r3.get_value(), &Some(2));
    }

    #[test]
    fn test_subtraction() {
        let r1: Ring<7> = Ring::new(2);
        let r2: Ring<7> = Ring::new(5);
        let r3 = r1 - r2;
        let r4 = r2 - r1;
        assert_eq!(r3.get_value(), &Some(4));
        assert_eq!(r4.get_value(), &Some(3));
    }

    #[test]
    fn test_multiplication() {
        let r1: Ring<6> = Ring::new(2);
        let r2: Ring<6> = Ring::new(3);
        let r3 = r1 * r2;
        assert_eq!(r3.get_value(), &Some(0));
    }

    #[test]
    fn test_division() {
        // 2 tests - one without inverse, one with inverse
        let r: Ring<10> = Ring::new(5);
        let zero: Ring<10> = Ring::new(0);
        let a = r / zero;
        assert_eq!(a.get_value(), &None);


        // let r1: Ring<6> = Ring::new(2);
        // let r2: Ring<6> = Ring::new(3);
        // let r3 = r1 / r2;
        // assert_eq!(r3.get_value(), &None);

        // let r4: Ring<7> = Ring::new(2);
        // let r5: Ring<7> = Ring::new(3);
        // let r6 = r4 / r5;
        // assert_eq!(r6.get_value(), &Some(3)); // 2 * 3^-1 mod 7 = 2 * 5 mod 7 = 3
    }

    #[test]
    fn test_negation() {
        let r1: Ring<5> = Ring::new(2);
        let r2 = -r1;
        assert_eq!(r2.get_value(), &Some(3));
    }

    #[test]
    fn test_assignments() {
        let mut r1: Ring<5> = Ring::new(2);
        let r2: Ring<5> = Ring::new(3);
        r1 += r2;
        assert_eq!(r1.get_value(), &Some(0));

        r1 *= r2;
        assert_eq!(r1.get_value(), &Some(0));

        r1 -= r2;
        assert_eq!(r1.get_value(), &Some(2));

        r1 /= r2;
        assert_eq!(r1.get_value(), &Some(4));
    }
}
