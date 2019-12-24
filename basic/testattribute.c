

#include <stdio.h>
#include <stdarg.h>

void myPrintf(const char *format, ...) __attribute((format(printf,1,2)));

int main(int argc, const char *argv[])
{
    unsigned i,j;

    i = 1;
    j = 2;
    printf("i=%d,j=%d\n", i, j);
    myPrintf("test %s\n", "hello");
    myPrintf("%s,%d,%d\n","hello", 1,2);
    return 0;
}


void myPrintf(const char *format, ...){

    char buf[1024];
    va_list ap;

    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    fprintf(stdout, "%s", buf);
}
