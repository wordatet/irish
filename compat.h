#ifndef COMPAT_H
#define COMPAT_H

#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <fnmatch.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>

/* IRIX/SVR4 types */
typedef unsigned char uchar_t;
typedef unsigned int uint_t;
typedef unsigned short ushort_t;
typedef unsigned long ulong_t;

/* Shims for pfmt/locale/setlabel */
#include <stdio.h>
#include <stdarg.h>

#define MM_ERROR 0
#define MM_WARNING 1
#define MM_INFO 2
#define MM_ACTION 3
#define MM_NOLABEL 0x10
#define MM_NOSTD 0x20
#define MAXLABEL 24

static inline int pfmt(FILE *stream, long flags, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vfprintf(stream, format, args);
    va_end(args);
    return ret;
}

static inline char *gettxt(const char *id, const char *def) {
    return (char *)def;
}

static inline int setlabel(const char *label) {
    return 0;
}

#define gettext(x) (x)
#define _gettext(x) (x)

/* Missing macros */
#ifndef MAXNAMLEN
#define MAXNAMLEN 255
#endif

#define setcat(x) (0)
#define setlabel(x) (0)

/* Linker shims */
int gmatch(const char *s, const char *p);
int sig2str(int sig, char *str);
int str2sig(char *str, int *sig);
int wisprint(int c);
int mbftowc(unsigned char *s, wchar_t *pwc, int (*f)(), int *p);

extern unsigned char **sh_environ;
extern const char *const _sys_siglist[];
extern int _sys_nsig;

#endif /* COMPAT_H */
#ifndef EFF_ONLY_OK
#define EFF_ONLY_OK 0
#endif
