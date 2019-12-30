

#ifndef __CONFIG_H
#define __CONFIG_H

#if defined(__APPLE__) || (defined(__linux__) && defined(__GLIBC__)) || \
    defined(__FreeBSD__) || (defined(__OpenBSD__) && defined(USE_BACKTRACE)) \
    || defined(__DragonFly__)
#define HAVE_BACKTRACE 1
#endif
