#ifndef __AE_H__
#define __AE_H__

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2
#define AE_BARRIER 4


struct aeEventLoop;
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef struct aeFileEvent {
    int mask;
    aeFileProc *rfileProc;
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

typedef struct aeFiredEvent {
    int fd;
    int mask;   
} aeFiredEvent;

typedef struct aeEventLoop {
    int maxfd;
    int setsize;
    aeFileEvent *events;
    aeFiredEvent *fired;
    void *apidata;
    int stop;
} aeEventLoop;

aeEventLoop *aeCreateEventLoop(int setsize);
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);
int aeCreateFileEvent(aeEventLoop *eventLoop,int fd, int mask,
        aeFileProc *proc, void *clientData);
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
int aeProcessEvents(aeEventLoop *eventLoop);
void aeMain(aeEventLoop *eventLoop);
#endif
