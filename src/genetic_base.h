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


typedef struct _individu{
    double fitness;
    point* points;    
} individu;

typedef struct _population{
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
int init_population(int size,int numpoints, polygon* poly , population** popu_ptr);

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
 * Free allocated meomory of the population including it's individu's
 * @param population pointer to what to clear;
 * @return 
 */
void free_population(population** population);

/**
 * Calculates the fitness of an individu and returns it
 * @param individu
 * @return 
 */
double get_fitness(population* population,individu* individu);

int get_best(population* population);

int do_iterations(population* population, int num_iterations);

/**
 * Does a random mutation and/or crossover on a select number of individu's
 * @param population
 * @return 
 */
int do_sex(population* population,int* lovers);

/**
 * Tries to select population->lovers random partners
 * @param population
 * @param indices
 * @return number of found partners
 */
int do_mate_selection(population* population,int* indices);

int do_crossover(population* population,individu* papa, individu* mama,individu* son, individu* daughter);

int do_mutation(population* population, individu* individu);

int do_deathmatch(population* plebs, int to_kill);

/**
 * 
 * @param plebs
 * @param group_size
 * @param over_size
 * @return ID to ill
 */
int do_tournament_selection(population* plebs, int group_size,int over_size);

void population_print(population* individu);

void individu_print(population* population,individu* individu);

#endif	/* GENETIC_BASE_H */

