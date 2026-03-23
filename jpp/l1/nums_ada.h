#pragma once

#ifndef LIBRARY_API
#define LIBRARY_API "ADA"
#endif
#define API_ADA

// Ada functions exported with pragma Export and C calling convention
extern void numsinit(void);
extern void numsfinal(void);

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

/* ada_ prefixed wrappers exported by Ada library */
extern int ada_gcd(int a, int b);
extern int ada_min_divider(int n);
extern int ada_totient(int n);
extern diofant_result_t ada_diofant(int a, int b, int c);
