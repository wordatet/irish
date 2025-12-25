#include	"defs.h"
#include    <stdlib.h>
#include    <string.h>

/*
 *	storage allocator
 *	Replaced with standard malloc/free for Linux port
 *  Wrapper functions sh_alloc/sh_free are used to maintain 
 *  compatibility with valid legacy shell calls.
 */

unsigned	brkincr = BRKINCR;

#ifdef __STDC__
void *
#else
char *
#endif
sh_alloc(nbytes)
	size_t nbytes;
{
	void *p;
	if (nbytes == 0) return NULL;
	
	p = malloc(nbytes);
	if (!p) {
		error(0, nospace, nospaceid);
	}
	return p;
}

void
sh_free(ap)
	void *ap;
{
	if (ap) {
#undef free
		free(ap); 
#define free sh_free
	}
}

/* 
 * addblok legacy stub
 */
void
addblok(reqd)
	unsigned reqd;
{
}

/* 
 * realloc wrapper
 * Used by __gtxt() in legacy code.
 */
void *
realloc(oldp, nbytes)
void *oldp;
size_t nbytes;
{
	if (!oldp) return sh_alloc(nbytes);
	if (nbytes == 0) {
		sh_free(oldp);
		return NULL;
	}
	/* Use standard libc realloc. 
	   We undefine realloc to bypass any potential macros (though none are currently defined). 
	   Note: This function is exported as 'realloc' to match the symbol expected by some legacy IRIX libs or tests,
	   even though defs.h maps realloc->sh_realloc for internal shell usage. */
#undef realloc
	return realloc(oldp, nbytes);
#define realloc sh_realloc
}

/* sh_realloc wrapper for internal shell references */
void *
sh_realloc(oldp, nbytes)
void *oldp;
size_t nbytes;
{
#undef realloc
    return realloc(oldp, nbytes);
#define realloc sh_realloc
}

#ifdef DEBUG
chkbptr(ptr)
	struct blk *ptr;
{
}

chkmem()
{
}
#endif
