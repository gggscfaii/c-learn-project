#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>

#include "ae.h"
#include "ae_select.c"


aeEventLoop *aeCreateEventLoop(int setsize) {
    aeEventLoop *eventLoop;
    int i;

    if((eventLoop = malloc(sizeof(*eventLoop))) == NULL) goto err;
    eventLoop->events = malloc(sizeof(aeFileEvent)*setsize);
    eventLoop->fired = malloc(sizeof(aeFiredEvent)*setsize);
    if(eventLoop->fired == NULL) goto err;
    eventLoop->maxfd = -1;
    eventLoop->setsize = setsize;
    if(aeApiCreate(eventLoop) == -1) goto err;
    for (int i = 0; i < setsize; i++) {
        eventLoop->events[i].mask = AE_NONE;
    }
    return eventLoop;
err:
    if(eventLoop) {
        free(eventLoop->fired);
        free(eventLoop);
    }
    return NULL;
}

void aeDeleteEventLoop(aeEventLoop *eventLoop) {
    aeApiFree(eventLoop);
    free(eventLoop->events);
    free(eventLoop->fired);
    free(eventLoop);
}

int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData)
{
    if(fd >= eventLoop->setsize){
        errno = ERANGE;
        return AE_ERR;
    }

    aeFileEvent *fe = &eventLoop->events[fd];
    if (aeApiAddEvent(eventLoop, fd, mask) == -1) {
        return AE_ERR;
    }
    fe->mask |= mask;
    if(mask & AE_READABLE) fe->rfileProc = proc;
    if(mask & AE_WRITABLE) fe->wfileProc = proc;
    fe->clientData = clientData;
    if(fd > eventLoop->maxfd)
        eventLoop->maxfd = fd;
    return AE_OK;
}

void aeStop(aeEventLoop *eventLoop) {
    eventLoop->stop = 1;
}

void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask)
{
    if (fd >= eventLoop->setsize) return;
    aeFileEvent *fe = &eventLoop->events[fd];
    if(fe->mask == AE_NONE) return;

    if(mask & AE_WRITABLE) mask |= AE_BARRIER;

    aeApiDelEvent(eventLoop,fd, mask);
    fe->mask = fe->mask & (~mask);
    if (fd == eventLoop->maxfd && fe->mask == AE_NONE) {
        int j;
        for(j = eventLoop->maxfd-1; j>=0; j--)
            if(eventLoop->events[j].mask != AE_NONE) break;
        eventLoop->maxfd = j;
    }
}

int aeProcessEvents(aeEventLoop *eventLoop)
{
    int processed = 0, numevents;

    numevents = aeApiPoll(eventLoop, NULL);

    for (int i = 0; i < numevents; i++) {
        aeFileEvent *fe = &eventLoop->events[eventLoop->fired[i].fd];
        int mask = eventLoop->fired[i].mask;
        int fd = eventLoop->fired[i].fd;

        if(fe->mask & mask & AE_READABLE) {
            fe->rfileProc(eventLoop, fd, fe->clientData, mask);
        }

        if(fe->mask & mask & AE_WRITABLE) {
            fe->wfileProc(eventLoop, fd, fe->clientData, mask);
        }

        processed++;
    }

    return processed;
}

void aeMain(aeEventLoop *eventLoop) {
    eventLoop->stop = 0;
    while(!eventLoop->stop) {
        aeProcessEvents(eventLoop);
    }
}
