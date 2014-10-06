

all: testcontains
maxdist: *.c
	mpicc  -Wall -std=c99 -DDEBUG -g  -o maxdist  *.c -I.
	
testcontains: *.c
	mpicc  -Wall -std=c99 -DDEBUG -DTESTINIT -g  -o maxdist  *.c -I.

	
clean: 
	rm -f  maxdist

