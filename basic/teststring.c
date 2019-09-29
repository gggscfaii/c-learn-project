#include <stdio.h>
#include <string.h>

struct {

    char name[30];
    int age;
} person, person_copy;

int main(int argc, const char *argv[])
{
    char myname[] = "pieree be fermat";

    /* using memcpy to copy string */
    memcpy(person.name, myname , sizeof(myname));
    person.age = 46;

    memcpy(&person_copy, &person, sizeof(person));

    printf("name = %s\n", person_copy.name);
    printf("age = %d\n", person_copy.age);

    char str[] = "memmove can be very useful....";
    memmove(str + 20, str + 15, 11);
    puts(str);

    char str1[] = "Sample string";
    char str2[40];
    char str3[40];
    char str4[40];
    strcpy(str2, str1);
    strcpy(str3, "copy successful");
    strncpy(str4, str1, 5);
    str[4] = '\0';
    strcat(str4, str1);
    strncat(str2, str1, 5);
    printf("str1:%s\nstr2:%s\nstr3:%s\nstr4:%s\n", str1, str2, str3, str4);

    char buf1[] = "DWgaOtP12df0";
    char buf2[] = "DWGAOTP12DF0";
    char *s;
    int n;
    n = memcmp(buf1, buf2, sizeof(buf1));
    printf("memcmp(%s, %s, sizeof(buf1)) %d\n", buf1, buf2, n);

    n = strcmp(buf1, buf2);
    printf("strcmp(%s, %s) %d\n", buf1, buf2, n);

    n = strncmp(buf1, buf2, 2);
    printf("strncmp(%s, %s,2) %d\n", buf1, buf2, n);

    s = memchr(buf1, '1', sizeof(buf1));
    printf("memchr(%s, %c, 3), %s\n", buf1, '1', s);

    s = strchr(buf1, '1');
    printf("strchr(%s, %c), %s\n", buf1, '1', s);

    return 0;
}
