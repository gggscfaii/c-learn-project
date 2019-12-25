
#include <stdio.h>

int main(int argc, const char *argv[])
{
    int rownum;
    printf("please input rowNum");
    scanf("%d", &rownum);
    
    for(int i=0; i< rownum; i++) {
        for(int j=0; j< rownum-i; j++){
            printf(" ");
        }
        for(int j = 0; j< 2*i+1; j++) {
            printf("*");
        }
        printf("\n");
    }


    for(int i=rownum -2 ; i>= 0; i--) {
        for(int j=0; j< rownum-i; j++){
            printf(" ");
        }
        for(int j = 0; j< 2*i+1; j++) {
            printf("*");
        }
        printf("\n");
    }
    return 0;
}
