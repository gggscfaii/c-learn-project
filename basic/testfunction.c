#include <stdio.h>

int x = 1;

void* testVoidReturnValue()
{
    return &x;
}

int main(int argc, const char *argv[])
{
    void* x = testVoidReturnValue();
    printf("x=%d\n", *(int*)x);
    return 0;
}
