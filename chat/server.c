

#include "server.h"
#include "list.h"
#include "anet.h"
#include "ae.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/types.h>
#include <signal.h>

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#ifndef __OpenBSD__
#include <ucontext.h>
#else
typedef ucontext_t sigcontext_t;
#endif

#endif
/*=================================Globals===============================*/

/* Global vars */
struct chatServer server;

int listenToPort(int port, int *fds, int *count) {

    int unsupported = 0;
    *count = 0;
    fds[*count] = anetTcp6Server(server.neterr, port, NULL, 128);
    if(fds[*count] != ANET_ERR) {
        anetNonBlock(NULL, fds[*count]);
        (*count)++;
    }
    else if(errno == EAFNOSUPPORT) {
        serverLog(LL_WARNING, "Not listening to IPv6:unsupported");
        unsupported++;
    }

    if (*count == 1 || unsupported) {
        fds[*count] = anetTcpServer(server.neterr, port, NULL, 128);
        if(fds[*count] != ANET_ERR) {
            anetNonBlock(NULL, fds[*count]);
            (*count)++;
        }
        else if(errno == EAFNOSUPPORT) {
            serverLog(LL_WARNING, "Not listening to IPv4:unsupported");
            unsupported++;
        }
    }

    if(*count + unsupported == 2) return C_OK;
    return C_ERR;
}

void initServer() {
    
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    setupSignalHandlers();
  
    server.syslog_enabled = 1; 
    server.logfile = strdup(CONFIG_DEFAULT_LOGFILE); 
    server.clients = listCreate();
    server.el = aeCreateEventLoop(1024); 
    if(listenToPort(6379, server.ipfd, &server.ipfd_count) == C_ERR)
        exit(1);

    if(server.ipfd_count == 0) {
        exit(1);
    }

    for (int i = 0; i < server.ipfd_count; i++) {
        if(aeCreateFileEvent(server.el, server.ipfd[i], AE_READABLE,
                    acceptTcpHandler, NULL) == AE_ERR) {
            
        }
    }
}

void daemonsize(void) {
    int fd;

    if(fork() != 0) exit(0);
    setsid();

    if((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if(fd > STDERR_FILENO) close(fd);
    }
}

int main(int argc, const char *argv[])
{

    initServer();
    
    daemonsize();
    
    aeMain(server.el);
    aeStop(server.el);
    return 0;
}

void joinChatGroupId(client *c, int chatGroupId) {
    c->chatGroupId = chatGroupId;
}

void sendMessageToClient(client *c, char *msg, int len){
    int nwritten;
    nwritten = write(c->fd, msg, len);
    if(nwritten == -1) {
        if(errno == EAGAIN) {
            nwritten = 0;
        }
        else {
            freeClientAsync(c);
        }
    }
}

void sendMessageToChatGroup(char *msg, int chatGroupId, int len) {
    client *c;
    listNode *node;
    node = server.clients->head;
    while(node != NULL) {
        c = (client*)node->value;
        if(c->chatGroupId == chatGroupId)
            sendMessageToClient(c, msg, len);
        node = node->next; 
    }
}

void serverLogRaw(int level, const char *msg) {
    const int sysLogLevelMap[] = {LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING};
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmod = (level & LL_RAW);
    int log_to_stdout = server.logfile[0] == '\0';

    level &= 0xff;
    if(level < server.verbosity) return;

    fp = log_to_stdout? stdout : fopen(server.logfile, "a");
    if(!fp) return;

    if (rawmod) {
       fprintf(fp, "%s", msg);
    }
    else {
       int off;
       time_t ctime;
       struct tm *tm;

       ctime = time(NULL);
       tm = localtime(&ctime);
       strftime(buf, sizeof(buf), "%d %b %Y %H:%M", tm);
       fprintf(fp, "%d %c %s %s", (int)getpid(), c[level], buf, msg);
    }
    fflush(fp);

    if(!log_to_stdout) fclose(fp);
    if(server.syslog_enabled)  syslog(sysLogLevelMap[level], "%s", msg);
}

void serverLog(int level, const char *fmt, ...) {
    va_list ap;
    char msg[LOG_MAX_LEN];

    if((level&0xff) < server.verbosity) return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    serverLogRaw(level,msg);
}

static void sigShutdownHandler(int sig) {
    char *msg;
    switch(sig) {
        case SIGINT:
            msg = "Received SIGINT scheduling shutdown...";
            break;
        case SIGTERM:
            msg = "Received SIGTERM scheduling shutdown...";
            break;
        default:
            msg = "Received shutdown signal, scheduling shutdown...";
    }

    if(server.shutdown_asap && sig == SIGINT) {
        serverLog(LL_WARNING, "You insis... exiting now.");
        exit(1);
    }
    
    serverLog(LL_WARNING, "%s", msg);
    server.shutdown_asap = 1;
}

int openDirectLogFiles(void) {
    int log_to_stdout = server.logfile[0] == '\0';
    int fd = log_to_stdout ?
        STDOUT_FILENO :
        open(server.logfile, O_APPEND|O_CREAT|O_WRONLY, 0644);
    return fd;
}

void closeDirectLogFiles(int fd) {
    int log_to_stdout = server.logfile[0] == '\0';
    if(!log_to_stdout) close(fd);
}

void logStackTrace(ucontext_t *uc) {
    void *trace[101];
    int trace_size = 0, fd = openDirectLogFiles();
    
    if(fd == -1) return;

    trace_size = backtrace(trace+1, 100);
    backtrace_symbols_fd(trace+1, trace_size, fd);
    closeDirectLogFiles(fd);
}

void sigsegvHandler(int sig, siginfo_t *info, void *secret) {
    ucontext_t *uc = (ucontext_t*)secret;
    struct sigaction act;

    serverLog(LL_WARNING, "Chat crashed by signal:%d", sig);
    if(sig == SIGSEGV || sig== SIGBUS) {
        serverLog(LL_WARNING,
        "Accessing addrss: %p", (void*)info->si_addr);
    }

    logStackTrace(uc);
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = SIG_DFL;
    sigaction(sig, &act, NULL);
    kill(getpid(), sig);
}

void setupSignalHandlers(void) {
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sigShutdownHandler;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);

#ifdef HAVE_BACKTRACE
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = sigsegvHandler;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGILL, &act, NULL);
#endif
}
