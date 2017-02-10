# make			Compile ${SRC} source file (not necessary)
# make t[est]		Syntax check of ${SRC}
# make s[afe[ness]]	Safeness test of ${SRC}
# make l[ive[ness]]	Liveness test of ${SRC}
# make p[rint]		Print out simulation trail
# make c[lean]		Clean temporary files
# make e[dit]		Edit ${SRC}

SRC=dijkstra

CC=gcc
CFLAGS=-pipe -O6 -funroll-loops

SPIN=spin
EDITOR=vi

all: ${SRC}.safeness ${SRC}.liveness

${SRC}.safeness: pan.c
	${CC} ${CFLAGS} -o $@ -DSAFETY pan.c # -DREACH

${SRC}.liveness: pan.c
	${CC} ${CFLAGS} -o $@ -DNP pan.c # -DREACH

t test pan.c: ${SRC}
	${SPIN} -a -v ${SRC}

s safe safeness: ${SRC}.safeness
	@rm -f ${SRC}.trail
	time ./${SRC}.safeness # -i
	@if [ -f ${SRC}.trail ]; then \
	     echo "*** Error: Use \"make print\" to see what happened" ; \
	fi

l live liveness: ${SRC}.liveness
	@rm -f ${SRC}.trail
	time ./${SRC}.liveness -l -f # -i
	@if [ -f ${SRC}.trail ]; then \
	     echo "*** Error: Use \"make print\" to see what happened" ; \
	fi

p print: ${SRC}
	-${SPIN} -t -p -g -l -w ${SRC}

c clean:
	rm -f pan.[bchmot] ${SRC}.safeness ${SRC}.liveness ${SRC}.trail

e edit:
	${EDITOR} ${SRC}

