

all: maxdist
maxdist: *.c
	mpicc  -Wall -std=c99 -DDEBUG  -g  -o maxdist  *.c -I. -lm

test: *.c
	mpicc  -Wall -std=c99 -DDEBUG -DTESPOPULATOR -g  -o maxdist  *.c -I. -lm

clean:
	rm -f  maxdist

