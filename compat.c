#include "compat.h"
#include <fnmatch.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

/* Global environment for renomed environ */
unsigned char **sh_environ;

/* Signal list shims */
const char *const _sys_siglist[] = {
    "EXIT", "HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "EMT", "FPE",
    "KILL", "BUS", "SEGV", "SYS", "PIPE", "ALRM", "TERM", "USR1", "USR2",
    "CHLD", "PWR", "WINCH", "URG", "IO", "STOP", "TSTP", "CONT", "TTIN",
    "TTOU", "VTALRM", "PROF", "XCPU", "XFSZ", "WAITING", "LWP", "FREEZE",
    "THAW", "CANCEL", "LOST", "USR1", "USR2", NULL
};
int _sys_nsig = 40;

int gmatch(const char *s, const char *p) {
    return fnmatch(p, s, FNM_PATHNAME) == 0;
}

int wisprint(int c) {
    return iswprint(c);
}

int mbftowc(unsigned char *s, wchar_t *pwc, int (*f)(), int *p) {
    /* Minimal stub for multibyte char reading */
    int c = f(p);
    if (c == EOF) return 0;
    *pwc = (wchar_t)c;
    s[0] = (unsigned char)c;
    s[1] = '\0';
    return 1;
}

int sig2str(int sig, char *str) {
    if (sig >= 0 && sig < _sys_nsig && _sys_siglist[sig]) {
        strcpy(str, _sys_siglist[sig]);
        return 0;
    }
    return -1;
}

int str2sig(char *str, int *sig) {
    int i;
    for (i = 0; i < _sys_nsig; i++) {
        if (_sys_siglist[i] && strcmp(str, _sys_siglist[i]) == 0) {
            *sig = i;
            return 0;
        }
    }
    return -1;
}
