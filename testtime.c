#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static void printTime(const struct tm *timeinfo);

int main(int argc, const char *argv[])
{
    time_t tm;
    struct tm *timeinfo;

    time(&tm);
    printTime(localtime(&tm));
    printTime(gmtime(&tm));

    timeinfo = malloc(sizeof(struct tm));
    timeinfo->tm_year = 2018 - 2000;
    timeinfo->tm_mon = 8;
    timeinfo->tm_mday = 13;

    mktime(timeinfo);
    printf("wk = %d\n", timeinfo->tm_wday);
    free(timeinfo);
    return 0;
}


static void printTime(const struct tm *timeinfo)
{
    char *buf;
    buf = malloc(80);
    strftime(buf, 80, "%Y-%m-%d %H:%M", timeinfo);
    printf("current:%s\n", buf);
    free(buf);
}


