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

extern "C" { 
    fn ada_gcd(a: i32, b: i32) -> i32; 
    fn ada_diofant(a:i32,b:i32,c:i32) -> diofant_result_t;
}


fn run_static_tests() {
    let tests = [ (48,18,6), (5,3,2), (3,2,1) ];
    println!("Combined Rust test harness: static + interactive (use 'interactive' arg)");

    unsafe {
        for (a,b,c) in tests.iter() {
            let a = *a; let b = *b; let c = *c;
            println!("C gcd({}, {}) = {}", a, b, gcd(a,b));
            let ada_g = ada_gcd(a,b);
            println!("Ada gcd({}, {}) = {}", a, b, ada_g);

            let res_c = diofant(a,b,c);
            let res_ada = ada_diofant(a,b,c);
            println!("C diofant -> x={}, y={}", res_c.x, res_c.y);
            println!("Ada diofant -> x={}, y={}", res_ada.x, res_ada.y);
            println!("---");
        }
    }
}

fn interactive_mode() {
    use std::io::{self, Write};
    println!("Interactive mode: enter 'a b c' lines; Ctrl+D to end.");
    let mut input = String::new();
    while { input.clear(); io::stdin().read_line(&mut input).unwrap_or(0) > 0 } {
        let parts: Vec<_> = input.trim().split_whitespace().collect();
        if parts.len() < 3 { println!("Please enter three integers a b c"); continue; }
        if let (Ok(a), Ok(b), Ok(c)) = (parts[0].parse::<i32>(), parts[1].parse::<i32>(), parts[2].parse::<i32>()) {
            unsafe {
                println!("C gcd({}, {}) = {}", a, b, gcd(a,b));
                println!("Ada gcd = {}", ada_gcd(a,b));
                let r_c = diofant(a,b,c);
                let r_ada = ada_diofant(a,b,c);
                println!("C diofant -> x={}, y={}", r_c.x, r_c.y);
                println!("Ada diofant -> x={}, y={}", r_ada.x, r_ada.y);
            }
        } else { 
            println!("Invalid integers, try again");
        }
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    run_static_tests();
    if args.len() > 1 && args[1] == "interactive" {
        interactive_mode();
    } else {
        println!("To run interactive tests, re-run: {} interactive", args.get(0).unwrap_or(&"./main_rust_combined".to_string()));
    }
}

