#pragma once

#define LIBRARY_API "RUST"

int gcd(int, int);
int min_divider(int);
int totient(int);

typedef struct {
    int x; int y;
} diofant_result_t;

// Solves a*x - b*y = c for x,y
diofant_result_t diofant(int a, int b, int c);