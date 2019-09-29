#include <stdio.h>


struct T{ double a[4];};

struct T f1(){ return (struct T){3.15};}

double g1(double *a){ return *a;}

void g2(double *a){ *a=1.0;}

int* foo()
{
    int a = 17;
    return &a;
}

int main(int argc, const char *argv[])
{
    int *p = foo();
//    int n = *p;

//    printf("%d", n);

    double d = g1(f1().a);
    printf("%.2lf\n", d);
    g2(f1().a);
    return 0;
}
