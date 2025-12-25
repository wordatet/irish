/*	Copyright (c) 1990, 1991 UNIX System Laboratories, Inc.	*/
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF     	*/
/*	UNIX System Laboratories, Inc.                     	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sh:error.c	1.9.12.1"
/*
 * UNIX shell
 */

#include	<stdio.h>
#include	<string.h>
#include	"defs.h"

static void com_failed();

extern int unlink();

/* ========	error handling	======== */

void
failed(cmd, s1, s2, s2id)
int cmd;
const char unsigned *s1;
const char	 *s2, *s2id;
{
	com_failed(cmd, 0, s1, s2, s2id);
	exitsh(ERROR);
}

void
error(cmd, s, sid)
int cmd;
const char	*s, *sid;
{
	failed(cmd, (const unsigned char *)s, NIL, NIL);
}

void
error_fail(cmd, s, sid)
int cmd;
const char	*s, *sid;
{
	failure(cmd, (const unsigned char *)s, NIL, NIL);
}

void
warning(cmd, s, sid)
int cmd;
const char	*s, *sid;
{
	com_failed(cmd, 1, (const unsigned char *)s, NIL, NIL);
}

void
exitsh(xno)
int	xno;
{
	/*
	 * Arrive here from `FATAL' errors
	 *  a) exit command,
	 *  b) default trap,
	 *  c) fault with no trap set.
	 *
	 * Action is to return to command level or exit.
	 */
	exitval = xno;
	flags |= eflag;
	if ((flags & (forcexit | forked | errflg | ttyflg)) != ttyflg)
		done(0);
	else
	{
		clearup();
		restore(0);
		(void)setb(1);
		execbrk = breakcnt = funcnt = 0;
		longjmp(errshell, 1);
	}
}

void
rmtemp(base)
struct ionod	*base;
{
	while (iotemp > base)
	{
		(void)unlink((char *)iotemp->ioname);
		free(iotemp->iolink);
		iotemp = iotemp->iolst;
	}
}

void
rmfunctmp()
{
	while (fiotemp)
	{
		(void)unlink((char *)fiotemp->ioname);
		fiotemp = fiotemp->iolst;
	}
}

void
failure(cmd, s1, s2, s2id)
int cmd;
const char unsigned *s1;
const char	 *s2, *s2id;
{
	com_failed(cmd, 0, s1, s2, s2id);
	if (flags & errflg)
		exitsh(ERROR);

	flags |= eflag;
	exitval = ERROR;
	exitset();
}

void
prusage(cmd, s, sid)
int cmd;
const char *s, *sid;
{
	flushb();
	(void)set_label(cmd);

	/* pfmt(stderr, MM_ERROR, badusage); */
	/* pfmt(stderr, MM_ACTION, usage, gettxt(sid, s)); */
	fprintf(stderr, "sh: usage: %s\n", s);

	if (flags & errflg)
		exitsh(ERROR);

	flags |= eflag;
	exitval = ERROR;
	exitset();
}

void
pr_usage(cmd, s, sid)
int cmd;
const char *s, *sid;
{
	flushb();
	(void)set_label(cmd);

	/* pfmt(stderr, MM_ERROR, badusage); */
	/* pfmt(stderr, MM_ACTION, usage, gettxt(sid, s)); */
	fprintf(stderr, "sh: usage: %s\n", s);
	exitsh(ERROR);
}

static void
com_failed(cmd, flag, s1, s2, s2id)
int cmd;
long flag;
const char unsigned *s1;
const char	 *s2, *s2id;
{
	flushb();
	(void)set_label(cmd);
	/* pfmt(stderr, flag, NULL); */
	/* flag 0 = error, 1 = warning */
	/* pfmt would print UX:sh: ERROR: ... */
	/* We simulate it simply: */
	/* fprintf(stderr, "sh: %s: ", flag ? "warning" : "error"); */
	/* Actually UnixWare/SVR4 prints the label set by set_label? */
	/* For now, just a prefix */
	
	/* if (!comb_label) prp(); */ /* prp prints prompt? or label? */
	/* The original log used pfmt to print headers. */
	
	/* simplified output */
	prs("sh: ");
	if (s2) {
		prs_cntl(s1);
		prs(": ");
		prs(s2);
	} else {
		prs(s1);
	}
	newline();
}

int
set_label(cmd)
int cmd;
{
	char *cmd_lbl = "UX:sh";
	char cmd_buf[MAXLABEL];
	int i, too_long;
	
	switch(cmd){
	case 0:
		break;
	case SYSFG:
		cmd_lbl = "UX:fg";
		break;
	case SYSBG:
		cmd_lbl = "UX:bg";
		break;
	case SYSTST:
		cmd_lbl = "UX:test";
		break;
	default:
		for (i = 0 ; i < no_commands ; i++){
			if (commands[i].sysval == cmd)
				break;
		}
		if (i != no_commands){
			(void)strcpy(cmd_buf, "UX:");
			(void)strncpy(cmd_buf+3, commands[i].sysnam,MAXLABEL-3);
			cmd_buf[MAXLABEL-1] = '\0';
			cmd_lbl = cmd_buf;
		}
	}

	if ((flags & prompt) == 0 && cmdadr){	/* Create a combined label */
		int len, len2;
		char *ptr;

		if (cmd_lbl != cmd_buf){
			(void)strcpy(cmd_buf, cmd_lbl);
			cmd_lbl = cmd_buf;
		}

		len = strlen(cmd_lbl) + 3;	/* For space and () */
		len2 = strlen((char *)cmdadr);


		if ((too_long = len + len2 >= MAXLABEL)
		    && (ptr = strrchr((char *)cmdadr, '/')) != 0) {

			len2 -= ++ptr - (char *)cmdadr;
			too_long = len + len2 >= MAXLABEL;

		} else
			ptr = (char *)cmdadr;

		if (!too_long) {
			(void)strcpy(cmd_buf + len - 3, " (");
			(void)strcpy(cmd_buf + len - 1, ptr);
			(void)strcpy(cmd_buf + len - 1 + len2, ")");

		} else {
			len = len > MAXLABEL ? MAXLABEL : len;
			(void)strcpy(cmd_buf + len - 3, " (");
			(void)strncpy(cmd_buf + len - 1, ptr, MAXLABEL- len);
			(void)strcpy(cmd_buf + MAXLABEL - 5, "...)");
		}
	}
	
	(void)setlabel(cmd_lbl);
	return 1;
}
