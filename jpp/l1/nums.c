#include "nums.h"

int gcd(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int min_divider(int n) {
    if (n % 2 == 0) {
        return 2;
    }

    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return i;
        }
    }
    return n;
}

int totient(int n) {
    int result = n;
    for (int p = 2; p * p <= n; ++p) {
        if (n % p == 0) {
            while (n % p == 0) {
                n /= p;
            }
            result -= result / p;
        }
    }
    if (n > 1) {
        result -= result / n;
    }
    return result;
}

// Solves a*x - b*y = c for x,y
diofant_result_t diofant(int a, int b, int c) {
    diofant_result_t result = {0, 0};
    int g = gcd(a, b);
    if (c % g != 0) {
        return result; // No solutions
    }
    a /= g;
    b /= g;
    c /= g;

    int orig_a = a; /* normalized a */
    int orig_b = b; /* normalized b */

    int x0 = 1, y0 = 0;
    int x1 = 0, y1 = 1;
    while (b != 0) {
        int q = a / b;
        int t = a % b;
        a = b;
        b = t;

        t = x0 - q * x1;
        x0 = x1;
        x1 = t;

        t = y0 - q * y1;
        y0 = y1;
        y1 = t;
    }

    /* particular solution */
    x0 = x0 * c;
    y0 = -y0 * c;

    /* search parameter k so that x = x0 + orig_b*k, y = y0 + orig_a*k are > 0 */
    int k;
    for (k = -10000; k <= 10000; k++) {
        int x = x0 + (orig_b * k);
        int y = y0 + (orig_a * k);
        if (x > 0 && y > 0) {
            result.x = x;
            result.y = y;
            return result;
        }
    }

    /* no positive solution found in range */
    return result;
}