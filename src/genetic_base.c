/* 
 * File:   genetic_base.h
 * Author: garonn
 *
 * Created on 06 October 2014, 22:00
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "genetic_base.h"
#include "settings.h"
#include "debugmacro.h"

/**
 * Creates an filed population of a certain size initited randomly
 * @param size number of individus
 * @param poly pointer to pollygon
 * @param population ponter that is returned (will be allocated)
 * @return 
 */
int init_population(int size, int numpoints, polygon* poly, population** popul) {
    int i;
    population* popu;

    *popul = (population*) malloc(sizeof (population));
    if (NULL == popul)
        return -1;
    popu = *popul;


    popu->polygon = poly;

    popu->numpoints = numpoints;
    popu->size = size;
    popu->lovers = ((int) (size * LOVER_PERCENT / 200)) *2; // force even

    popu->list = (individu*) malloc(sizeof (individu) * (size + popu->lovers));
    if (NULL == popu->list) {
        free(popu);
        return -2;
    }

    popu->_allpoints = (point*) malloc(sizeof (point) * (size + popu->lovers) *  numpoints);
    for (i = 0; i < size + popu->lovers; i++) {
        init_individu(popu, popu->list + i,popu->_allpoints + (i * numpoints));
    }




    return 0;
}

/**
 * Intialize individu with random points and allocate space
 * @param numpoints number of points
 * @param individu ppointer to store stuff in
 * @return 0 on succes and -1 on failed malloc
 */
int init_individu(population* popu, individu* solution, point* point_ptr) {
    solution->points = point_ptr;
    solution->population = popu;
    polygon_random_points(popu->numpoints, solution->points, popu->polygon);
    return 0;
}

/**
 * Free allocated meomory of the population including it's individu's
 * @param population pointer to what to clear;
 * @return 
 */
void free_population(population** popu) {
    free((*popu)->_allpoints);
    free((*popu)->list);
    free(*popu);
    *popu = NULL;
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

int get_best(population* population) {
    int best = 0;
    int i;
    for (i = 1; i < population->size; i++) {
        if (population->list[i].fitness > population->list[best].fitness)
            best = i;

    }

    return best;

}

int do_iterations(population* population, int num_iterations) {
    int *lovers = (int*) malloc(population->lovers * sizeof (int));
    int i, j;
    double avg;
    double prev_avg = 0.0;
    log_dbg("|  GEN  | FITNESS |   DELTA  |\n");
    for (i = 0; i < num_iterations; i++) {
        do_sex(population, lovers);
        do_deathmatch(population, population->lovers);

        avg = 0.0;
        for (j = 0; j < population->size; j++) {
            avg += population->list[i].fitness;
        }
        avg = avg / (double) population->size;
        log_dbg("|  %-3d  | % 3.3f | %+3.3f |\n", i, avg, avg - prev_avg);
        prev_avg = avg;

    }


    free(lovers);
    return 0;
}

/**
 * Does a random mutation and/or crossover on a select number of individu's
 * @param population
 * @return 
 */
int do_sex(population* population, int* lovers) {
    int size = population->size;
    individu* list = population->list;
    int i;

    do_mate_selection(population, lovers);

    for (i = 0; i < population->lovers; i += 2) {
        do_crossover(
                population,
                list + lovers[i], list + lovers[i + 1],
                list + size + i, list + size + i + 1
                );


        if (rand() % MUTATION_1_IN == 0) { // MOVE
            do_mutation(list + size + i);
        }
        if (rand() % MUTATION_1_IN == 0) { // MOVE
            do_mutation(list + size + i + 1);
        }

        get_fitness(list + size + i);
        get_fitness(list + size + i + 1);
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
    int i=0;
    double total_fitness = 0.0;
    double offset=0.0, interval=0.0, target=0.0, counter=0.0;
    int curIndex = 0;

    //Calculate the total fitness
    for (i = 0; i < population->size; i++) {
        total_fitness += get_fitness(population->list + i);
    }

    //set the interval:
    interval = total_fitness / (double) population->lovers;

    //Make random offset
    offset = (double) rand() * interval / (double) RAND_MAX;

    target = offset;

    for (i = 0; i < population->size; i++) {

        counter += population->list[i].fitness;
        if (counter >= target) {
            indices[curIndex] = i;
            curIndex++;
            if (curIndex >= population->lovers)
                break;
            target += interval;
        }

    }

    //Shuffel list
    // NO NEED

    return curIndex;
}

int do_crossover(population* population, individu* papa, individu* mama, individu* son, individu* daughter) {
    int num = population->numpoints;
    int split = 1 + (rand() % (num - 1));
    int i;

    //2 times for efficiency less checks
    for (i = 0; i < split; i++) {
        son->points[i] = papa->points[i];
        daughter->points[i] = mama->points[i];
    }
    for (i = i; i < population->numpoints; i++) {
        son->points[i] = mama->points[i];
        daughter->points[i] = papa->points[i];
    }
    return 0;
}

int do_mutation(individu* individu) {
    int randindex = rand() % individu->population->numpoints;
    float baseX = individu->points[randindex].x;
    float baseY = individu->points[randindex].y;
    float new_x, new_y;
    float max_delta = individu->population->polygon->diagonal / (float) MUTATION_DELTA;
    do {
        max_delta = max_delta / 2.0;
        if (max_delta < FLT_MIN) {
            log_dbg("FAILED to mutate\n");
            return -1;
        }
        new_x = baseX + max_delta * ((double) rand() / (double) RAND_MAX)*(rand() % 2 ? 1.0 : -1.0);
        new_y = baseY + max_delta * ((double) rand() / (double) RAND_MAX)*(rand() % 2 ? 1.0 : -1.0);
    } while (!polygon_contains(new_x, new_y, individu->population->polygon));

    individu->points[randindex].x = new_x;
    individu->points[randindex].y = new_y;

    return 0;
}

int do_deathmatch(population* plebs, int to_kill) {
    int left, victim, i;
    int group_size = (plebs->size + plebs->lovers) * 100 / SELECTION_PRESSUERE;
    for (left = to_kill; left >= 0; left--) {
        victim = do_tournament_selection(plebs, group_size, left);

        //printf("We'll kill nr %d or %p", victim ,plebs->list + victim);
        //individu_print(plebs->list + victim);

        /* plebs[victim] to be replaced by  plebs[size + left - 1] */
        plebs->list[victim].fitness = plebs->list[plebs->size + left - 1].fitness;
        /*
                memcpy(
                        plebs->list[victim].points,
                        plebs->list[plebs->size + left - 1].points,
                        plebs->numpoints * sizeof (point)
                        );
         */
        for (i = 0; i < plebs->numpoints; i++) {
            plebs->list[victim].points[i] = plebs->list[plebs->size + left - 1].points[i];
        }

        //individu_print(plebs->list + victim);
        //individu_print(plebs->list + plebs->size + left - 1);

        //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n %d left ~~~~",left);
    }

    return left;
}

int do_tournament_selection(population* plebs, int group_size, int over_size) {
    int population_size = plebs->size + over_size;
    int worst_ID = rand() % population_size;
    int i, cur_id;
    for (i = 0; i < group_size; i++) {
        cur_id = rand() % population_size;
        if (plebs->list[cur_id].fitness < plebs->list[worst_ID].fitness)
            worst_ID = cur_id;
    }

    return worst_ID;
}

void population_print(population* population) {
    int i;
    printf("Printing population of size %d\n", population->size);

    for (i = 0; i < population->size; i++) {
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



