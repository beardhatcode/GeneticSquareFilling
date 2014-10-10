/* 
 * Header file for the GA
 * 
 * File:   genetic_base.h
 * Author: Robbert Gurdeep Singh
 *
 * Created on 06 October 2014, 22:00
 */

#ifndef GENETIC_BASE_H
#define	GENETIC_BASE_H

#include "polygon.h"
#include "settings.h"

typedef struct _individu
{
    double fitness;
    point* points;
} individu;

typedef struct _population
{
    individu* list;
    int numpoints;
    int num_lovers;
    int size;
    polygon* polygon;

    point* _allpoints; /* pointer for easy freeing*/
} population;

/**
 * Creates an empty population of a certain size
 * @param size           Number of individus in the population
 * @param numpoints      Number of points per individu
 * @param poly           The polygon this population is in
 * @param popu_ptr       A pointer to a pointer to store stuff in
 * @return 0 on succed, negative on failed alloc
 *         -1 : failed to alloc population
 *         -2 : failed to alloc Array of individu's
 *         -3 : failed to alloc point array
 */
int init_population(int size, int numpoints, polygon* poly, population** popu_ptr);

/**
 * Initialize a solution with a pointer to it's points array 
 * and fill this array randomly
 * @param popu          population the individu is in
 * @param solution      pointer to the meomory location of the indivdual
 * @param point_ptr     pointer to the start of the array of points
 * @return 0 on succes
 */
int init_individu(population* popu, individu* solution, point* point_ptr);

/**
 * Free allocated meomory of the population including it's individus
 * and their points
 * @param population    pointer to free
 */
void free_population(population** population);

/**
 * (Re)Calculates the fitness of an individu stores it for later use
 * and returns it.
 * 
 * Use individu->fitness to get value if you know it's calculated
 * 
 * @param individu
 * @return 
 */
double get_fitness(population* population, individu* individu);

/**
 * Does a linear search for the best individu in the population
 * @param population    population too find best in
 * @return index of the best in population->list
 */
int get_best(population* population);

/**
 * Fakes num_generations generations of evolution
 * 
 * @param population        the populationin that will evolve
 * @param num_iterations    number of generations
 * @todo  enable auto averaging
 * @return 0 on success,
 *        -1 on failed allocation of array of indices
 */
int do_iterations(population* population, int num_iterations);

/**
 * Forms pairs and creates children (that may mutate) and add them after the
 * population->list array (that is indices [size, size+num_lovers - 1])
 * @param population        the population to work with
 * @return number of generated children
 */
int do_sex(population* population, int* lovers);

/**
 * Tries to select population->lovers random partners from the population using
 * Stochastic universal sampling. 
 * @todo  write about no shuffle
 * @param population    population to work with
 * @param indices       array to store indices in (existing allocated pointer)
 * @return number of found partners
 */
int do_mate_selection(population* population, int* indices);

/**
 * Does a one point crossover of mama and papa into son and daughter (pointers)
 * Note: get_fitness MUST be run on the children after this operation
 * @param population    population we are working with
 * @param papa          parent 1
 * @param mama          parent 2
 * @param son           child 1
 * @param daughter      child 2
 * @return 0 on success
 */
int do_crossover(population* population, individu* papa, individu* mama, individu* son, individu* daughter);

/**
 * Picks a random point from the child and modifies it a little bit
 * Note: get_fitness MUST be run on the children after this operation
 * @param population    population we are working with
 * @param individu      the individu from the population that needs to be mutated
 * @return 0 on success; -1 on failed mutation
 */
int do_mutation(population* population, individu* individu);

int do_deathmatch(population* plebs, int to_kill);

/**
 * 
 * @param plebs
 * @param group_size
 * @param over_size
 * @return ID to ill
 */
int do_tournament_selection(population* plebs, int group_size, int over_size);

void population_print(population* individu);

void individu_print(population* population, individu* individu);

#endif	/* GENETIC_BASE_H */

