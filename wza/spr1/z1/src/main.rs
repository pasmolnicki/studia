use z1::*;

fn main() {
    const A: IntegerType = 2;
    const B: IntegerType = 8;
    const C: IntegerType = 3;
    const D: IntegerType = 9;
    const E: IntegerType = 9;
    const F: IntegerType = 9;

    println!("N({} + {}i) = {}", A, B, GaussianType::new(A, B).norm());
    println!("({} + {} + ({} + {}i)) / ({} + {}i) = {}", C, A, B, D, E, F, GaussianType::new(C + A, B + D) / (GaussianType::new(E, F)));

    // (3 + 2) + (8 + 9)i = 5 + 17i
    // (5 + 17i) / (9 + 9i) = [(5 + 17i)(9 - 9i)] / (9^2 + 9^2) = [(45 + 153) + (153 - 45)i] / 162 = [198 + 108i] / 162 = 1 + i
    // All possible results: 
    // ...
}
