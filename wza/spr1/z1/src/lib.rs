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

pub fn gcd_list(numbers: &[GaussianType]) -> Option<Vec<GaussianType>> {
    if numbers.is_empty() {
        return None;
    }
    
    // NWD pojedynczego elementu to on sam
    let mut current_gcd = numbers[0];
    for &num in numbers.iter().skip(1) {
        current_gcd = gcd(current_gcd, num);
    }
    
    // Z uwagi na to, że w Z[i] NWD jest znany tylko z dokładnością do elementów
    // odwracalnych wygenerujemy wszystkie możliwe asocjanty: x * 1, x * -1, x * i, x * -i
    // (To właśnie jest powód wszystkich "możliwych rozwiązań" do wypisania)
    let unit_1 = GaussianType::new(1, 0);       // 1
    let unit_min1 = GaussianType::new(-1, 0);   // -1
    let unit_i = GaussianType::new(0, 1);       // i
    let unit_mini = GaussianType::new(0, -1);   // -i

    Some(vec![
        current_gcd * unit_1,
        current_gcd * unit_min1,
        current_gcd * unit_i,
        current_gcd * unit_mini,
    ])
}

pub fn lcm_list(numbers: &[GaussianType]) -> Option<Vec<GaussianType>> {
    if numbers.is_empty() {
        return None;
    }
    
    let mut current_lcm = numbers[0];
    for &num in numbers.iter().skip(1) {
        // NWW(a, b) dla Gaussa w oparciu o to same twierdzenie: ab / NWD(a,b)
        current_lcm = lcm(current_lcm, num);
    }
    
    let unit_1 = GaussianType::new(1, 0);       // 1
    let unit_min1 = GaussianType::new(-1, 0);   // -1
    let unit_i = GaussianType::new(0, 1);       // i
    let unit_mini = GaussianType::new(0, -1);   // -i

    // NWW tak jak NWD jest definiowane do czynika równego relacjom odwracalnym (asocjatom) w okół jedności
    Some(vec![
        current_lcm * unit_1,
        current_lcm * unit_min1,
        current_lcm * unit_i,
        current_lcm * unit_mini,
    ])
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

    fn div(self, other: Self) -> Self {
        let denominator = other.norm();
        let numerator = self * other.conjugate();

        Self {
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

    fn units() -> [GaussianType; 4] {
        [
            GaussianType::new(1, 0),
            GaussianType::new(-1, 0),
            GaussianType::new(0, 1),
            GaussianType::new(0, -1),
        ]
    }

    fn is_associate(a: GaussianType, b: GaussianType) -> bool {
        units().iter().any(|u| a == b * *u)
    }

    fn is_zero(z: GaussianType) -> bool {
        z == GaussianType::new(0, 0)
    }

    fn assert_four_associates(values: &[GaussianType], base: GaussianType) {
        assert_eq!(values.len(), 4);
        for u in units() {
            assert!(values.iter().any(|v| *v == base * u));
        }
    }

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
    fn test_gcd_divides_both_and_is_correct_up_to_unit() {
        let a = GaussianType::new(6, 0);
        let b = GaussianType::new(4, 0);
        let g = gcd(a, b);

        assert!(is_zero(a.div_mod(g).1));
        assert!(is_zero(b.div_mod(g).1));
        assert!(is_associate(g, GaussianType::new(2, 0)));
    }

    #[test]
    fn test_lcm_is_multiple_of_both_and_matches_gcd_relation() {
        let a = GaussianType::new(6, 0);
        let b = GaussianType::new(4, 0);
        let g = gcd(a, b);
        let l = lcm(a, b);

        assert!(is_zero(l.div_mod(a).1));
        assert!(is_zero(l.div_mod(b).1));
        assert!(is_associate(g * l, a * b));
    }

    #[test]
    fn test_gcd_nontrivial_numbers_with_common_nontrivial_factor() {
        let a = GaussianType::new(3, 5);
        let b = GaussianType::new(1, 3);
        let g = gcd(a, b);

        assert!(is_zero(a.div_mod(g).1));
        assert!(is_zero(b.div_mod(g).1));
        assert!(is_associate(g, GaussianType::new(1, 1)));
    }

    #[test]
    fn test_gcd_div_mod_half_tie_rounding_edge_cases() {
        // (5+6i)/(1+i) = 5.5 + 0.5i, ties should round away from zero.
        let a1 = GaussianType::new(5, 6);
        let b1 = GaussianType::new(1, 1);
        let (q1, r1) = a1.div_mod(b1);
        assert_eq!(q1, GaussianType::new(6, 1));
        assert!(r1.norm() < b1.norm());
        assert_eq!(gcd(a1, b1).norm(), 1);

        // (5+4i)/(1+i) = 4.5 - 0.5i, negative half also rounds away from zero.
        let a2 = GaussianType::new(5, 4);
        let b2 = GaussianType::new(1, 1);
        let (q2, r2) = a2.div_mod(b2);
        assert_eq!(q2, GaussianType::new(5, -1));
        assert!(r2.norm() < b2.norm());
        assert_eq!(gcd(a2, b2).norm(), 1);
    }

    #[test]
    fn test_gcd_list_empty_and_singleton_nontrivial() {
        assert_eq!(gcd_list(&[]), None);

        let z = GaussianType::new(3, 4);
        let res = gcd_list(&[z]).expect("single element list should return associates");
        assert_four_associates(&res, z);
    }

    #[test]
    fn test_gcd_list_nontrivial_multi_element_maximality() {
        let numbers = [
            GaussianType::new(3, 5),
            GaussianType::new(1, 3),
            GaussianType::new(5, 3),
        ];
        let res = gcd_list(&numbers).expect("non-empty list should return associates");
        let g = res[0];

        assert_four_associates(&res, g);
        for n in numbers {
            assert!(is_zero(n.div_mod(g).1));
        }

        let reduced: Vec<GaussianType> = numbers.iter().map(|n| n.div_mod(g).0).collect();
        let reduced_gcd = reduced.iter().copied().skip(1).fold(reduced[0], gcd);
        assert_eq!(reduced_gcd.norm(), 1);
    }

    #[test]
    fn test_lcm_list_empty_and_singleton_nontrivial() {
        assert_eq!(lcm_list(&[]), None);

        let z = GaussianType::new(3, 4);
        let res = lcm_list(&[z]).expect("single element list should return associates");
        assert_four_associates(&res, z);
    }

    #[test]
    fn test_lcm_list_nontrivial_multi_element_properties() {
        let numbers = [
            GaussianType::new(2, 3),
            GaussianType::new(-3, 2),
            GaussianType::new(-2, -3),
        ];
        let res = lcm_list(&numbers).expect("non-empty list should return associates");
        let l = res[0];

        assert_four_associates(&res, l);
        for n in numbers {
            assert!(is_zero(l.div_mod(n).1));
        }

        let common_multiple = numbers
            .iter()
            .copied()
            .fold(GaussianType::new(1, 0), |acc, n| acc * n);
        assert!(is_zero(common_multiple.div_mod(l).1));
    }
}