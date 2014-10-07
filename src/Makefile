

all: testrandomgenerator
maxdist: *.c
	mpicc  -Wall -std=c99 -DDEBUG -g  -o maxdist  *.c -I.
	
testcontains: *.c
	mpicc  -Wall -std=c99 -DDEBUG -DTESTINIT -g  -o maxdist  *.c -I.

testrandomgenerator: *.c
	mpicc  -Wall -std=c99 -DDEBUG -DTESTRAND -g  -o maxdist  *.c -I.
	
clean: 
	rm -f  maxdist

