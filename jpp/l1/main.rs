#[repr(C)]
pub struct diofant_result_t {
    pub x: i32,
    pub y: i32,
}

extern "C" {
    pub fn gcd(a: i32, b: i32) -> i32;
    pub fn min_divider(n: i32) -> i32;
    pub fn totient(n: i32) -> i32;
    pub fn diofant(a: i32, b: i32, c: i32) -> diofant_result_t;
}


fn main() {
    let a = 48;
    let b = 18;
    let c = 6;

    println!("GCD of {} and {} is {}", a, b, unsafe { gcd(a, b) });
    println!("Minimum divider of {} is {}", a, unsafe { min_divider(a) });
    println!("Totient of {} is {}", a, unsafe { totient(a) });

    let result = unsafe { diofant(a, b, c) };
    if result.x == 0 && result.y == 0 {
        println!("No solutions for the equation {}x - {}y = {}", a, b, c);
    } else {
        println!("One solution for the equation {}x - {}y = {} is x = {}, y = {}", a, b, c, result.x, result.y);
    }
}

