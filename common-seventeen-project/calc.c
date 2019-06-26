#include <stdio.h>

typedef struct stack{
    stack *next;
    char *val;
} stack;


stack *push(stack *head, stack *e)
{
    if (head != NULL) {
        e->next = head;
    }
    head = e;
    return head;
}

stack *pop(stack *head)
{
    stack *e = head;
    head=head->next;
    return e;
}

int len(stack *head)
{
    statck *e;
    int len;

    e = head;
    len = 0;
    while (e != NULL) {
        len++;
        e = e->next;
    }

    return len;
}
