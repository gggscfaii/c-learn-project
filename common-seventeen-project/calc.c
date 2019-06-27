#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct stack{
    struct stack *next;
    char val;
} stack;


stack *init(int n)
{
    stack *s = malloc(sizeof(stack));
    s->next = NULL;
    return s;
}

stack *push(stack *head, char c)
{
    stack *e = malloc(sizeof(stack));
    e->val = c;

    if (head != NULL) {
        e->next = head;
    }
    head = e;
    return head;
}

char pop(stack *head)
{
    stack *e = head;
    head=head->next;
    return e->val;
}


char *ptos(char *s)
{

}


int main(int argc, const char *argv[])
{
    char *a = argv[1];
    int n = strlen(a);

    stack *s = init(n);
    for (int i = 0; i < n; i++) {
        if (a[i] == ')') {
            printf("%c ",pop(s));
        }
        else if(a[i] == '+' || a[i] == '*'){
            push(s, a[i]);
        }
        else if(a[i] >= '0' && a[i] <= '9'){
            printf("%c ",a[i]);
        }
    }
    printf("\n");

    return 0;
}
