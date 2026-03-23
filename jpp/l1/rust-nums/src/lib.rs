
#[unsafe(no_mangle)]
pub extern "C" fn gcd(a: i32, b: i32) -> i32 {
    let mut a = a;
    let mut b = b;

    while b != 0 {
        let temp = b;
        b = a % b;
        a = temp;
    }
    a
}

#[unsafe(no_mangle)]
pub extern "C" fn min_divider(n: i32) -> i32 {
    for i in 2..=n {
        if n % i == 0 {
            return i;
        }
    }
    n
}

#[unsafe(no_mangle)]
pub extern "C" fn totient(n: i32) -> i32 {
    let mut n = n;
    let mut result = n;
    let mut p = 2;
    while p * p <= n {
        if n % p == 0 {
            while n % p == 0 {
                n /= p;
            }
            result -= result / p;
        }
        p += 1;
    }
    if n > 1 {
        result -= result / n;
    }
    result
}

#[repr(C)]
pub struct diofant_result_t {
    pub x: i32,
    pub y: i32,
}

#[unsafe(no_mangle)]
/// Solves the eqation ax - by = c
pub extern "C" fn diofant(a: i32, b: i32, c: i32) -> diofant_result_t {
    println!("Rust: a = {}, b = {}, c = {}", a, b, c);

    let mut a = a;
    let mut b = b;
    let mut c = c;
    let g = gcd(a, b);

    if c % g != 0 {
        return diofant_result_t { x: 0, y: 0 };
    }
    a /= g;
    b /= g;
    c /= g;

    let orig_a = a;
    let orig_b = b;

    let mut x0 = 1;
    let mut y0 = 0;
    let mut x1 = 0;
    let mut y1 = 1;

    while b != 0 {
        let q = a / b;
        let r = a % b;
        a = b;
        b = r;

        let x_temp = x1;
        let y_temp = y1;
        x1 = x0 - q * x1;
        y1 = y0 - q * y1;
        x0 = x_temp;
        y0 = y_temp;
    }

    x0 = x0 * c;
    y0 = -y0 * c;

    for k in -10000..=10000 {
        let x = x0 + orig_b * k;
        let y = y0 + orig_a * k;
        if x > 0 && y > 0 {
            return diofant_result_t { x, y };
        }
    }

    diofant_result_t { x: 0, y: 0 }
}
