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
    int result = 1;
    for (int i = 2; i < n; i++) {
        if (gcd(i, n) == 1) {
            result++;
        }
    }
    return result;
}

// Extended Euclidean Algorithm to find gcd and coefficients
int extended_gcd(int a, int b, int *x, int *y) {
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }
    int x1, y1;
    int gcd = extended_gcd(b % a, a, &x1, &y1);

    *x = y1 - (b / a) * x1;
    *y = x1;

    return gcd;
}

diofant_result_t diofant(int a, int b, int c) {
    int x0, y0;
    int g = extended_gcd(a, b, &x0, &y0);
    diofant_result_t result;

    // If g does not divide c, no integer solution exists
    if (c % g != 0) {
        result.x = -1;
        result.y = -1;
        return result;
    }

    // Initial solution for ax + by = c
    int x = x0 * (c / g);
    int y = -y0 * (c / g);

    int step_x = b / g;
    int step_y = a / g;

    while (x < 0 || y < 0) {
        x += step_x;
        y += step_y;
    }

    while (x - step_x >= 0 && y - step_y >= 0) {
        x -= step_x;
        y -= step_y;
    }

    result.x = x;
    result.y = y;
    return result;
}