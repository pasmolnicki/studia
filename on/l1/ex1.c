#include <stdio.h>
#include <float.h>


int main(int argc, char** argv)
{
    printf("eps double: %.17g\n", DBL_EPSILON);
    printf("esp float: %.17g\n", FLT_EPSILON);
    printf("max double: %.17g\n", DBL_MAX);
    printf("max float: %.17g\n", FLT_MAX);
    return 0;
}
