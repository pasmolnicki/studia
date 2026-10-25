#include <stdio.h>
#include <float.h>


int main(int argc, char** argv)
{
    printf("eps double: %e\n", DBL_EPSILON);
    printf("esp float: %e\n", FLT_EPSILON);
    printf("max double: %e\n", DBL_MAX);
    printf("max float: %e\n", FLT_MAX);
    return 0;
}
