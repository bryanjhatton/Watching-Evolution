CC = g++
CFLAGS = -c

all: watchingevolution

watchingevolution: main.o Clock.o Interface.o Evolve.o
	${CC} Clock.o Interface.o Evolve.o main.o -o watchingevolution

main.o: main.cpp
	${CC} ${CFLAGS} main.cpp

Evolve.o: Evolve.cpp
	${CC} ${CFLAGS} Evolve.cpp

Interface.o: Interface.cpp
	${CC} ${CFLAGS} Interface.cpp

Clock.o: Clock.cpp
	${CC} ${CFLAGS} Clock.cpp

clean:
	rm -rf *.o watchingevolution