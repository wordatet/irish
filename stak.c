/*
 * UNIX shell
 */

#include	"defs.h"
#include    <stdlib.h>

/* ========	storage allocation	======== */

#define STAK_SIZE (8 * 1024 * 1024) /* 8MB Fixed Stack */

static unsigned char *stak_buffer = NULL;

/* Initialize the stack buffer if not already done */
static void
init_stak(void)
{
	if (!stak_buffer) {
		stak_buffer = (unsigned char *)malloc(STAK_SIZE);
		if (!stak_buffer) {
			error(0, nospace, nospaceid);
		}
		stakbas = stakbot = staktop = stak_buffer;
		brkend = stak_buffer + STAK_SIZE;
	}
}

#ifdef __STDC__
void *
#else
unsigned char *
#endif
getstak(asize)			/* allocate requested stack */
int	asize;
{
	register unsigned char	*oldstak;
	register int	size;

	if (!stak_buffer) init_stak();

	size = round(asize, ALIGN);
	
	if (stakbot + size >= brkend) {
		error(0, nospace, nospaceid);
	}

	oldstak = stakbot;
	staktop = stakbot += size;
	return(oldstak);
}

/*
 * set up stack for local use
 * should be followed by `endstak'
 */
unsigned char *
locstak()
{
	if (!stak_buffer) init_stak();
	
	if (brkend - stakbot < BRKINCR)
	{
		/* In fixed mode, we can't grow. If we are running out, we are out. */
		/* But usually locstak just checks enough space for next op. */
		/* We rely on the 8MB being enough. */
		if (stakbot >= brkend)
			error(0, nospace, nospaceid);
	}
	return(stakbot);
}

unsigned char *
savstak()
{
	if (!stak_buffer) init_stak();
	assert(staktop == stakbot);
	return(stakbot);
}

unsigned char *
endstak(argp)		/* tidy up after `locstak' */
register unsigned char	*argp;
{
	register unsigned char	*oldstak;

	if (!stak_buffer) init_stak();

	*argp++ = 0;
	oldstak = stakbot;
	stakbot = staktop = (unsigned char *)round(argp, ALIGN);
	
	if (stakbot >= brkend) {
		error(0, nospace, nospaceid);
	}
	
	return(oldstak);
}

void
tdystak(x)		/* try to bring stack back to x */
register unsigned char	*x;
{
	if (!stak_buffer) init_stak();
	
	while ((unsigned char *)stakbsy > x)
	{
		free(stakbsy);
		stakbsy = stakbsy->ag.word;
	}
	staktop = stakbot = max(x, stakbas);
	rmtemp(x);
}

void
stakchk()
{
	if (!stak_buffer) init_stak();
	
	/* No-op in fixed stack mode as we don't shrink/grow brk */
}

#ifdef __STDC__
void *
#else
unsigned char *
#endif
cpystak(x)
unsigned char	*x;
{
	return(endstak(movstr(x, locstak())));
}
