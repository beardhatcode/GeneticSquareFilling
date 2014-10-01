

all: maxdist
maxdist: *.c
	mpicc  -Wall -std=c99 -DDEBUG -g  -o maxdist  *.c -I.
	
clean: 
	rm -f  maxdist

