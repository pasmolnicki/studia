#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define LIBRARY_API "RUST"

// Rust functions exported with #[no_mangle] and extern "C"
int gcd(int a, int b);
int min_divider(int n);
int totient(int n);

typedef struct {
    int x;
    int y;
} diofant_result_t;

diofant_result_t diofant(int a, int b, int c);

#ifdef __cplusplus
}
#endif
