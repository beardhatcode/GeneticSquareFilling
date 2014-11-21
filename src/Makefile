GENETICS=genetic_base.c polygon.c
OPTIONS=


all: maxdist
maxdist: *.c
	#Compiling maxdist with $(OPTIONS)
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist  main.c $(GENETICS) -I. -lm
	
maxdist_mpi: *.c
	#Compiling maxdist_mpi with $(OPTIONS)
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist_mpi  mpi_main.c $(GENETICS) -I. -lm

both: *.c
	#Compiling both with $(OPTIONS)
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist  main.c $(GENETICS) -I. -lm
	mpicc  -Wall -std=c99 -g $(OPTIONS) -o maxdist_mpi  mpi_main.c $(GENETICS) -I. -lm

test: *.c
	mpicc  -Wall -std=c99 -DDEBUG -DTESPOPULATOR -g  -o maxdist  *.c -I. -lm

clean:
	rm -f  maxdist maxdist_mpi

