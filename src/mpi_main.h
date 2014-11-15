/* 
 * File:   mpi_main.h
 * Author: garonn
 *
 * Created on 05 November 2014, 21:54
 */

#ifndef MPI_MAIN_H
#define	MPI_MAIN_H
#include "genetic_base.h"
#include "polygon.h"
#define  MASTER		0

#define GENETIC_CONTINUE 0x6060
#define GENETIC_NOT_BEST 0xFA11
#define GENETIC_BEST     0xB00B5
 
typedef int (*loop_checker)(population*, int, int, int);

/**
 * Parse the input from the program and return numpoints and the polygon
 * @param argc The number arguments given to the program
 * @param argv The arguments given to the program
 * @param poly  A pointer to where the polygon should be stored
 * @param num   The number of points
 * @return 
 */
int mpi_parse_input(int argc, char *argv[], polygon* poly, int* num);


/**
 * Called to start master
 * should call parrallel_loops
 * @param poly          polygon 
 * @param numpoints     number of points to place
 * @param id            procces id
 * @return 
 */
int master_main(polygon* poly, int numpoints, int id);

/**
 * Called to start slave
 * should call parrallel_loops
 * @param poly          polygon 
 * @param numpoints     number of points to place
 * @param id            procces id
 * @return 
 */
int  slave_main(polygon* poly, int numpoints, int id);



/**
 * Run the default program on poly for numpoints. 
 * After every itter loops call end loop with number of done iterations
 * and a pointer to the population
 * @param poly          a pointer tot the polygon
 * @param numpoints     number of points
 * @param id            The processor id
 * @param pop_size      population size
 * @param itters        number of iterations
 * @param end_loop      function to call if loop ends
 * @return 0 on succes
 */
int parrallel_loops(polygon* poly, int numpoints, int id, int pop_size, int itters, loop_checker end_loop);



/**
 * Transfer individus between prosesses
 * Simply calls transfer_individus_island of transfer_individus_gather .
 * @param popu      Population of current proccess
 * @param task_id   Numer of current procces
 * @return 0 on succes else mpi error
 */
int transfer_individus(population * popu, int task_id);
/*
 * Transfer individus between prosesses by sending to a limited number of 
 * processes
 * @param popu      Population of current proccess
 * @param task_id   Numer of current procces
 * @return 0 on succes else mpi error
 */
int transfer_individus_gather(population * popu, int task_id);
 
/*
 * Transfer individus between prosesses by sending to a limited number of 
 * processes
 * @param popu      Population of current proccess
 * @param task_id   Numer of current procces
 * @return 0 on succes else mpi error
 */
int transfer_individus_island(population * popu, int task_id);




#endif	/* MPI_MAIN_H */

