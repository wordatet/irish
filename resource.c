/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley Software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ident	"$Revision: 1.10 $"



#include "defs.h"
#include <sys/resource.h>
#include <sys/types.h>

#include <unistd.h>

static struct limits *findlim(unsigned char *);
static int64_t getval (register struct limits *lp, unsigned char **v);
static int limtail(unsigned char *, unsigned char *);
static int plim(register struct limits *lp, char hard);
static int setlim(register struct limits *lp, char hard, rlim_t limit);
static int psecs(int64_t);
static int p2dig(int64_t);
static int prefix(unsigned char *, unsigned char *);


static int command;
static struct limits { 
	int limconst; 
	unsigned char *limname; 
	int limdiv; 
	char *limscale; 
} limits[] = { 
	{RLIMIT_CPU, (unsigned char *)"cputime", 1, "seconds"}, 
	{RLIMIT_FSIZE, (unsigned char *)"filesize", 1024, "kbytes"}, 
	{RLIMIT_DATA, (unsigned char *)"datasize", 1024, "kbytes"}, 
	{RLIMIT_STACK, (unsigned char *)"stacksize", 1024, "kbytes"}, 
	{RLIMIT_CORE, (unsigned char *)"coredumpsize", 1024, "kbytes"}, 
	{RLIMIT_RSS, (unsigned char *)"memoryuse", 1024, "kbytes"}, 
	{RLIMIT_NOFILE, (unsigned char *)"descriptors", 1, ""}, 
	{RLIMIT_AS, (unsigned char *)"vmemory", 1024, "kbytes"}, 
	{-1, 0, 0, 0} 
};

static struct limits *
findlim(cp)
	unsigned char *cp;
{
	register struct limits *lp, *res;

	res = 0;
	for (lp = limits; lp->limconst >= 0; lp++)
		if (prefix(cp, lp->limname)) {
			if (res)
				error(command, ambiguous, ambiguousid);
			res = lp;
		}
	if (res)
		return (res);
	error(command, nosuchres, nosuchresid);
	return(0);
}

int
dolimit(v)
	register unsigned char **v;
{
	register struct limits *lp;
	register rlim_t limit;
	char hard = 0;

	command = SYSLIMIT;
	v++;
	if (*v && eq(*v, "-h")) {
		hard = 1;
		v++;
	}
	if (*v == 0) {
		for (lp = limits; lp->limconst >= 0; lp++)
			plim(lp, hard);
		return(0);
	}
	lp = findlim(v[0]);
	if (v[1] == 0) {
		plim(lp,  hard);
		return(0);
	}
	limit = getval(lp, v+1);
	if (setlim(lp, hard, limit) < 0)
		exitsh(ERROR);
}

static int64_t
getval(lp, v)
	register struct limits *lp;
	unsigned char **v;
{
	register double f;
	unsigned char *cp = *v++;

	f = atof((const char *)cp);
	while (digit(*cp) || *cp == '.' || *cp == 'e' || *cp == 'E')
		cp++;
	if (*cp == 0) {
		if (*v == 0)
			return ((int64_t)(f+0.5) * lp->limdiv);
		cp = *v;
	}
	switch (*cp) {

	case ':':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		return ((int64_t)(f * 60.0 + atof((const char *)cp+1)));

	case 'h':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		limtail(cp, (unsigned char *)"hours");
		f *= 3600.;
		break;

	case 'm':
		if (lp->limconst == RLIMIT_CPU) {
			limtail(cp, (unsigned char *)"minutes");
			f *= 60.;
			break;
		}
	case 'M':
		if (lp->limconst == RLIMIT_CPU)
			goto badscal;
		*cp = 'm';
		limtail(cp, (unsigned char *)"megabytes");
		f *= 1024.*1024.;
		break;

	case 's':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		limtail(cp, (unsigned char *)"seconds");
		break;

	case 'k':
		if (lp->limconst == RLIMIT_CPU)
			goto badscal;
		limtail(cp, (unsigned char *)"kbytes");
		f *= 1024;
		break;

	case 'u':
		limtail(cp, (unsigned char *)gettxt(":557", "unlimited"));
		return (RLIM_INFINITY);

	default:
badscal:
		error(command, badscale, badscaleid);
	}
	return ((int64_t)(f+0.5));
}

static
limtail(cp, str0)
	unsigned char *cp, *str0;
{
	register unsigned char *str = str0;

	while (*cp && *cp == *str)
		cp++, str++;
	if (*cp)
		error(command, badscale, badscaleid);
	return(0);
}

static
plim(register struct limits *lp, char hard)
{
	struct rlimit rlim;
	rlim_t limit;

	prs_buff(lp->limname);
	prs_buff(" \t");
	(void) getrlimit(lp->limconst, &rlim);
	limit = hard ? rlim.rlim_max : rlim.rlim_cur;
	if (limit == RLIM_INFINITY)
		prs_buff(gettxt(":557", "unlimited"));
	else if (lp->limconst == RLIMIT_CPU)
		psecs(limit);
	else {
		prn_buff(limit/lp->limdiv);
		prc_buff(' ');
		prs_buff(lp->limscale);
	}
	prc_buff(NL);
	return(0);
}

int
dounlimit(v)
	register unsigned char **v;
{
	register struct limits *lp;
	int err = 0;
	char hard = 0;

	command = SYSUNLIMIT;
	v++;
	if (*v && eq(*v, "-h")) {
		hard = 1;
		v++;
	}
	if (*v == 0) {
		for (lp = limits; lp->limconst >= 0; lp++)
			if (setlim(lp, hard, (rlim_t)RLIM_INFINITY) < 0)
				err++;
		if (err)
			exitsh(ERROR);
		return(0);
	}
	while (*v) {
		lp = findlim(*v++);
		if (setlim(lp, hard, (rlim_t)RLIM_INFINITY) < 0)
			exitsh(ERROR);
	}
}

static
setlim(register struct limits *lp, char hard, rlim_t limit)
{
	struct rlimit rlim;

	(void) getrlimit(lp->limconst, &rlim);
	if (hard)
		rlim.rlim_max = limit;
  	else if (limit == RLIM_INFINITY && geteuid() != 0)
 		rlim.rlim_cur = rlim.rlim_max;
 	else
 		rlim.rlim_cur = limit;
	if (setrlimit(lp->limconst, &rlim) < 0) {
		unsigned char errstr[50], *cp = errstr;

		cp = movstr(lp->limname, cp);
		cp = movstr(": Can't ", cp);
		if (limit == RLIM_INFINITY)
			cp = movstr("remove ", cp);
		else
			cp = movstr("set ", cp);
		if (hard)
			cp = movstr("hard ", cp);
		cp = movstr("limit\n", cp);
		prs(errstr);
		return (-1);
	}
	return (0);
}

static
psecs(int64_t l)
{
	register int64_t i;

	i = l / 3600;
	if (i) {
		prn_buff(i);
		prc_buff(':');
		i = l % 3600;
		p2dig((int64_t)(i / 60));
		goto minsec;
	}
	i = l;
	prn_buff(i / 60);
minsec:
	i %= 60;
	prc_buff(':');
	p2dig(i);
}

static
p2dig(register int64_t i)
{

	prn_buff((int64_t)(i / 10));
	prn_buff((int64_t)(i % 10));
	return(0);
}

static
prefix(sub, str)
	register unsigned char *sub, *str;
{

	for (;;) {
		if (*sub == 0)
			return (1);
		if (*str == 0)
			return (0);
		if (*sub++ != *str++)
			return (0);
	}
}



