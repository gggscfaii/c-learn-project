

#include <stdlib.h>
#include <malloc.h>
#include "list.h"

list *listCreate(void)
{
    struct list *list;

    if((list = malloc(sizeof(*list))) == NULL)
        return NULL;
    list->head = list->tail = NULL;
    list->len = 0;
    list->free = NULL;
    return list;
}


void listDelNode(list *list, listNode *node)
{
    if(node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    if(node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    if(list->free) list->free(node->value);
    free(node);
    list->len--;
}

listNode *listAddNodeHead(list *list, void *value)
{
    listNode *node;
   
    if((node=malloc(sizeof(*node))) == NULL)
        return NULL;

    if(list->len == 0) {
        list->tail = list->head = node;
        node->prev = NULL;
        node->next = NULL;
    }
    else {
        node->next = list->head;
        node->prev = NULL;
        list->head->prev = node;
        list->head = node;
    }
    list->len++;
    return node;
}

listNode *listAddNodeTail(list *list, void *value)
{
    listNode *node;
    
    if((node=malloc(sizeof(*node))) == NULL)
        return NULL;

    if(list->len == 0) {
        list->tail = node;
        list->head = node;
        node->prev = list->tail = NULL;
    }
    else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail = node;
        list->tail->next = node;
    }
    list->len++;
    return node;
}

