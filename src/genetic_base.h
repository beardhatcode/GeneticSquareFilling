/* 
 * File:   genetic_base.h
 * Author: garonn
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
    struct _population* population;
    
} individu;

typedef struct _population{
    double best_fitness;
    individu* best;
    individu* list; //size = size + lovers
    int numpoints;
    int lovers; // EVEN!!!
    int size;
    polygon* polygon;
} population;

/**
 * Creates an empty population of a certain size
 * @param size number of individus
 * @param poly pointer to pollygon
 * @param population ponter that is returned (will be allocated)
 * @return 
 */
int init_population(int size,int numpoints, polygon* poly , population** population);

/**
 * Intialize individu with random points and allocate space
 * @param numpoints number of points
 * @param individu ppointer to store stuff in
 * @return 
 */
int init_individu(population* popu, individu* solution);


/**
 * Free allocated meomory of the population including it's individu's
 * @param population pointer to what to clear;
 * @return 
 */
void free_population(population** population);

/**
 * It frees Willy
 * @param willy
 * @return 
 */
void free_individu(individu* willy);

/**
 * Calculates the fitness of an individu and returns it
 * @param individu
 * @return 
 */
double get_fitness(individu* individu);

/**
 * Does a random mutation and/or crossover on a select number of individu's
 * @param population
 * @return 
 */
int do_sex(population* population);

/**
 * Selects a biased random mate
 * @param population
 * @return 
 */
int do_mate_selection(population* population,int* indices);

int do_crossover(population* population,individu* papa, individu* mama,individu* son, individu* daughter);

int do_mutation(individu* individu);

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

void individu_print(individu* individu);

#endif	/* GENETIC_BASE_H */

