/* Combined C test: calls Ada wrappers (ada_*) and Rust wrappers (rust_*)
   Provides static unit tests for gcd, min_divider, totient and diofant,
   and also an optional interactive mode. */

#include <stdio.h>
#include <string.h>
#include "nums.h"
#include "nums_rust.h"
#include "nums_ada.h"

static void run_static_tests(void) {
    printf("Running static tests for C, Rust and Ada libraries\n");

    /* gcd tests: (a,b) -> expected */
    struct { int a, b, expected; } gcd_tests[] = {
        {48, 18, 6}, {7, 3, 1}, {100, 25, 25}
    };

    for (size_t i = 0; i < sizeof(gcd_tests)/sizeof(gcd_tests[0]); ++i) {
        int a = gcd_tests[i].a;
        int b = gcd_tests[i].b;
        int e = gcd_tests[i].expected;
        int r_c = gcd(a,b);
        int r_rust = rust_gcd(a,b);
        int r_ada = ada_gcd(a,b);
        printf("gcd(%d,%d): C=%d Rust=%d Ada=%d expected=%d\n", a,b,r_c,r_rust,r_ada,e);
    }

    /* min_divider tests */
    struct { int n, expected; } md_tests[] = { {15,3}, {16,2}, {17,17} };
    for (size_t i=0;i<sizeof(md_tests)/sizeof(md_tests[0]);++i) {
        int n = md_tests[i].n;
        printf("min_divider(%d): C=%d Rust=%d Ada=%d expected=%d\n", n, min_divider(n), rust_min_divider(n), ada_min_divider(n), md_tests[i].expected);
    }

    /* totient tests */
    struct { int n, expected; } phi_tests[] = { {1,1}, {9,6}, {10,4} };
    for (size_t i=0;i<sizeof(phi_tests)/sizeof(phi_tests[0]);++i) {
        int n = phi_tests[i].n;
        printf("totient(%d): C=%d Rust=%d Ada=%d expected=%d\n", n, totient(n), rust_totient(n), ada_totient(n), phi_tests[i].expected);
    }

    /* diofant tests: pick tuples with known positive solutions */
    struct { int a,b,c; int ex, ey; } d_tests[] = { {48,18,6,2,5}, {5,3,2,1,1}, {3,2,1,1,1} };
    for (size_t i=0;i<sizeof(d_tests)/sizeof(d_tests[0]);++i) {
        int a=d_tests[i].a, b=d_tests[i].b, c=d_tests[i].c;
        diofant_result_t rc = diofant(a,b,c);
        diofant_result_t rr = rust_diofant(a,b,c);
        diofant_result_t ra = ada_diofant(a,b,c);
        printf("diofant(%d,%d,%d): C=(%d,%d) Rust=(%d,%d) Ada=(%d,%d) expected=(%d,%d)\n",
               a,b,c, rc.x,rc.y, rr.x,rr.y, ra.x,ra.y, d_tests[i].ex, d_tests[i].ey);
    }
}

static void interactive_mode(void) {
    printf("Interactive mode: enter a b c (a*x - b*y = c), or EOF to quit:\n");
    int a,b,c;
    while (scanf("%d %d %d", &a, &b, &c) == 3) {
        printf("C gcd(%d,%d) = %d\n", a,b, gcd(a,b));
        printf("Rust gcd = %d, Ada gcd = %d\n", rust_gcd(a,b), ada_gcd(a,b));
        diofant_result_t rc = diofant(a,b,c);
        diofant_result_t rr = rust_diofant(a,b,c);
        diofant_result_t ra = ada_diofant(a,b,c);
        if (rc.x || rc.y) printf("C diofant -> x=%d y=%d\n", rc.x, rc.y); else printf("C diofant -> no positive solution\n");
        if (rr.x || rr.y) printf("Rust diofant -> x=%d y=%d\n", rr.x, rr.y); else printf("Rust diofant -> no positive solution\n");
        if (ra.x || ra.y) printf("Ada diofant -> x=%d y=%d\n", ra.x, ra.y); else printf("Ada diofant -> no positive solution\n");
        printf("---\n");
    }
}

int main(int argc, char **argv) {
    printf("Combined C test harness: static + interactive (use 'interactive' arg)\n");
    run_static_tests();

    if (argc > 1 && strcmp(argv[1], "interactive") == 0) {
        interactive_mode();
    } else {
        printf("To run interactive tests, re-run with: %s interactive\n", argv[0]);
    }
    return 0;
}