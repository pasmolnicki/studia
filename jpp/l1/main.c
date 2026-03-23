#include <stdio.h>

// #include "nums.h"
// #include "nums_rust.h"
#include "nums_ada.h"

int main() {
    printf(LIBRARY_API "\n");

    int a, b, c;
    printf("Enter a, b, c for the equation a*x - b*y = c: ");
    scanf("%d %d %d", &a, &b, &c);

#ifdef API_ADA
    numsinit();
#endif

    diofant_result_t result = diofant(a, b, c);
    if (result.x == 0 && result.y == 0) {
        printf("No solutions.\n");
    } else {
        printf("One solution is x = %d, y = %d\n", result.x, result.y);
    }

#ifdef API_ADA
    numsfinal();
#endif

    return 0;
}