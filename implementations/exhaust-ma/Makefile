# $Id: Makefile,v 1.7 2003/07/13 10:23:18 martinus Exp $

CC = cc
CFLAGS = ${OPT} ${DBG}
OPT = -O -fomit-frame-pointer
#DBG = -g -DDEBUG=2

# Recommended extra options for gcc:
#OPT += -fomit-frame-pointer -fforce-addr -finline-functions -funroll-loops
#OPT += -mcpu=i686 -march=i686
#DBG += -W -Wall -pedantic -ansi

LD = 
EXECUTABLES = exhaust

all: ${EXECUTABLES}

exhaust: sim.o asm.o pspace.o exhaust.c
	${CC} ${CFLAGS} -o exhaust sim.o asm.o pspace.o exhaust.c ${LD}

asm.o:	asm.c
	${CC} ${CFLAGS} -c asm.c

pspace.o: pspace.c
	${CC} ${CFLAGS} -c pspace.c

sim.o:	sim.c
	${CC} ${CFLAGS} -c sim.c

clean:
	rm -f *~ *.o core ${EXECUTABLES}
