#pragma once

#define LIBRARY_API "ADA"
#define API_ADA

// Ada functions exported with pragma Export and C calling convention
extern void numsinit(void);
extern void numsfinal(void);

extern int gcd(int a, int b);
extern int min_divider(int n);
extern int totient(int n);

typedef struct {
    int x;
    int y;
} diofant_result_t;

extern diofant_result_t diofant(int a, int b, int c);
