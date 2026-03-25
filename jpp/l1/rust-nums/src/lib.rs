
// Internal Rust implementations (not exported with C symbols)
pub fn gcd_impl(mut a: i32, mut b: i32) -> i32 {
    while b != 0 {
        let temp = b;
        b = a % b;
        a = temp;
    }
    a
}

pub fn min_divider_impl(n: i32) -> i32 {
    for i in 2..=n {
        if n % i == 0 {
            return i;
        }
    }
    n
}

pub fn totient_impl(mut n: i32) -> i32 {
    let mut result = 1;
    let mut p = 2;
    while p <= n {
        if gcd_impl(p, n) == 1 {
            result += 1;
        }
        p += 1;
    }

    result
}

#[repr(C)]
pub struct diofant_result_t {
    pub x: i32,
    pub y: i32,
}

/// Extended Euclidean Algorithm
/// Returns (gcd, x, y) such that ax + by = gcd
fn extended_gcd(a: i32, b: i32) -> (i32, i32, i32) {
    if a == 0 {
        return (b, 0, 1);
    }
    let (gcd, x1, y1) = extended_gcd(b % a, a);
    
    let x = y1 - (b / a) * x1;
    let y = x1;
    
    (gcd, x, y)
}

fn diofant_impl(a: i32, b: i32, c: i32) -> diofant_result_t {
    let (g, x0, y0) = extended_gcd(a, b);

    // Check if solution exists
    if c % g != 0 {
        return diofant_result_t { x: 0, y: 0 };
    }

    // Scale to c and account for the minus sign (ax - by = c)
    let mut x = x0 * (c / g);
    let mut y = -y0 * (c / g);

    let step_x = b / g;
    let step_y = a / g;

    // Shift to find smallest non-negative x and y
    while x < 0 || y < 0 {
        x += step_x;
        y += step_y;
    }

    while x - step_x >= 0 && y - step_y >= 0 {
        x -= step_x;
        y -= step_y;
    }

    diofant_result_t { x, y }
}

/* Small C ABI wrappers with distinct symbols so programs can link multiple libraries
   without symbol collisions. */
// For static:
// #[export_name = "rust_gcd"]
#[no_mangle]
pub extern "C" fn rust_gcd(a: i32, b: i32) -> i32 {
    gcd_impl(a, b)
}

#[export_name = "rust_min_divider"]
#[no_mangle]
pub extern "C" fn rust_min_divider(n: i32) -> i32 {
    min_divider_impl(n)
}

#[export_name = "rust_totient"]
#[no_mangle]
pub extern "C" fn rust_totient(n: i32) -> i32 {
    totient_impl(n)
}

#[export_name = "rust_diofant"]
#[no_mangle]
pub extern "C" fn rust_diofant(a: i32, b: i32, c: i32) -> diofant_result_t {
    diofant_impl(a, b, c)
}

