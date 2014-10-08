/* 
 * File:   genetic_base.h
 * Author: garonn
 *
 * Created on 06 October 2014, 22:00
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "genetic_base.h"
#include "settings.h"

/**
 * Creates an filed population of a certain size initited randomly
 * @param size number of individus
 * @param poly pointer to pollygon
 * @param population ponter that is returned (will be allocated)
 * @return 
 */
int init_population(int numpoints, polygon* poly, population* popu) {
    int i;

    popu->polygon = poly;
    popu->numpoints = numpoints;
    popu->best_fitness = -1.0;
    popu->best = NULL;

    for (i = 0; i < NUM_INDIVIDUS; i++) {
        init_individu(popu, popu->list + i);
    }

    return 0;
}

/**
 * Intialize individu with random points and allocate space
 * @param numpoints number of points
 * @param individu ppointer to store stuff in
 * @return 0 on succes and -1 on failed malloc
 */
int init_individu(population* popu, individu* solution) {
    solution->points = (point*) malloc(popu->numpoints * sizeof (point));
    if (NULL == solution->points)
        return -1;
    solution->population = popu;
    polygon_random_points(popu->numpoints, solution->points, popu->polygon);
    return 0;
}

/**
 * Free allocated meomory of the population including it's individu's
 * @param population pointer to what to clear;
 * @return 
 */
void free_population(population* popu) {
    int i;
    for (i = 0; i < NUM_INDIVIDUS; i++) {
        free_individu(&(popu->list[i]));
    }
}

/**
 * free the point array of the individu
 * @param willy
 * @return 
 */
void free_individu(individu* willy) {
    free(willy->points);
}

/**
 * Calculates the fitness of an individu and returns it
 * @param individu
 * @return 
 */
double get_fitness(individu* indi) {
    int i, j;
    double result = 0.0;
    point* l = indi->points;
    for (i = 0; i < indi->population->numpoints; i++) {
        for (j = 0; j < indi->population->numpoints; j++) {
            result += sqrt(sqrt((l[i].x - l[j].x)*(l[i].x - l[j].x)+(l[i].y - l[j].y)*(l[i].y - l[j].y)));
        }
    }
    indi->fitness = result;
    return result;
}

/**
 * Does a random mutation and/or crossover on a select number of individu's
 * @param population
 * @return 
 */
int do_sex(population* population) {
    int lovers[NUM_LOVERS] = {-1};
    do_mate_selection(population, lovers);
    int i;
    for (i = 0; i < NUM_INDIVIDUS; i++) {
        printf("Person %d :    (%f)\n", i,population->list[i].fitness);

    }

    for (i = 0; i < NUM_LOVERS; i++) {
        printf("Lover %d :%d  (%f)\n", i, lovers[i], population->list[lovers[i]].fitness);

    }

    return -1;
}

/**
 * Selects a biased random mate
 * @param population
 * @param indices array of int[NUM_LOVERS] 
 * @return 
 */
int do_mate_selection(population* population, int* indices) {
    int i;
    double total_fitness = 0.0;
    double offset, interval, target, counter;
    int curIndex = 0;

    //Calculate the total fitness
    for (i = 0; i < NUM_INDIVIDUS; i++) {
        total_fitness += get_fitness(population->list + i);
    }

    //set the interval:
    interval = total_fitness / (double) NUM_LOVERS;

    //Make random offset
    offset = (double) rand() * interval / (double) RAND_MAX;

    target = offset;

    for (i = 0; i < NUM_INDIVIDUS; i++) {

        counter += population->list[i].fitness;
        if (counter >= target) {
            indices[curIndex] = i;
            curIndex++;
            if (curIndex >= NUM_LOVERS)
                break;
            target += interval;
        }

    }

    return 0;
}

int do_crossover(individu* papa, individu* mama) {
    return -1;
}

int do_mutation(individu* individu) {
    return -1;
}

void population_print(population* population) {
    int i;
    printf("Printing population of size %d\n", NUM_INDIVIDUS);

    for (i = 0; i < NUM_INDIVIDUS; i++) {
        get_fitness(population->list + i);
        individu_print(population->list + i);
    }
}

void individu_print(individu* individu) {
    int i;
    printf("Individu %p: %f\n", individu, individu->fitness);
    for (i = 0; i < individu->population->numpoints; i++) {
        printf("\t%f\t%f\n", individu->points[i].x, individu->points[i].y);
    }

}



