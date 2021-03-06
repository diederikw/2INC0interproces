#
#	Makefile for 2INC0 Interprocess Communication
#
#	(c) TUe 2010-2016, Joris Geurts
#

BINARIES = worker farmer

CC = gcc
CFLAGS = -Wall -g -c -std=gnu89
LDLIBS = -lrt -lX11

all:	$(BINARIES)

clean:
	rm -f *.o $(BINARIES)

worker: worker.o 

farmer: farmer.o output.o

worker.o: worker.c settings.h common.h

output.o: output.c output.h settings.h

farmer.o: farmer.c output.h settings.h common.h

