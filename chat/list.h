#ifndef __ADLIST_H__
#define __ADLIST_H__

typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;

typedef struct list {
    listNode *head;
    listNode *tail;
    void (*free)(void *ptr);
    unsigned long len;
} list;

#define listSetFreeMethod(l,m) ((l)->free = (m))

list *listCreate(void);
void listDelNode(list *list, void *value);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);

#endif
