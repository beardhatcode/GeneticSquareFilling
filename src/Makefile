GENETICS=genetic_base.c polygon.c
OPTIONS= 
all: maxdist
maxdist: *.c
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist  main.c $(GENETICS) -I. -lm
	
maxdist_mpi: *.c
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist_mpi  mpi_main.c $(GENETICS) -I. -lm

both: *.c
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist  main.c $(GENETICS) -I. -lm
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist_mpi  mpi_main.c $(GENETICS) -I. -lm

test: *.c
	mpicc  -Wall -std=c99 -DDEBUG -DTESPOPULATOR -g  -o maxdist  *.c -I. -lm

clean:
	rm -f  maxdist maxdist_mpi

