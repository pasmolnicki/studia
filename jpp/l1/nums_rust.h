#pragma once

#ifndef LIBRARY_API
#define LIBRARY_API "RUST"
#endif

// Rust functions exported with #[no_mangle] and extern "C"
extern int gcd(int a, int b);
extern int min_divider(int n);
extern int totient(int n);

#ifndef DIOFANT_RESULT_T_DEFINED
#define DIOFANT_RESULT_T_DEFINED
typedef struct {
    int x;
    int y;
} diofant_result_t;
#endif

extern diofant_result_t diofant(int a, int b, int c);

/* wrappers provided by rust library with rust_ prefix */
extern int rust_gcd(int a, int b);
extern int rust_min_divider(int n);
extern int rust_totient(int n);
extern diofant_result_t rust_diofant(int a, int b, int c);

