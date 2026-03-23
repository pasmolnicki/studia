#pragma once

#define LIBRARY_API "C"

int gcd(int, int);
int min_divider(int);
int totient(int);

#ifndef DIOFANT_RESULT_T_DEFINED
#define DIOFANT_RESULT_T_DEFINED
typedef struct {
    int x; int y;
} diofant_result_t;
#endif

// Solves a*x - b*y = c for x,y
diofant_result_t diofant(int a, int b, int c);