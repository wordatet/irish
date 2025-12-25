# Makefile for porting IRIX 6.5.17 sh to Linux

CC = gcc
CFLAGS = -g -Wall -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -I. -DDIRTY -D_sgi -Dsgi
LDFLAGS = 

OBJS = args.o blok.o cmd.o ctype.o defs.o echo.o error.o expand.o \
       fault.o func.o hash.o hashserv.o io.o macro.o main.o msg.o name.o \
       print.o pwd.o service.o stak.o string.o test.o word.o \
       xec.o bltin.o jobs.o ulimit.o resource.o compat.o

PROG = sh

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(PROG)
