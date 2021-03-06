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

int init_population(int size, int numpoints, polygon* poly, population** popu_ptr)
{
    int i;
    population* popu;

    /* Allocate space to point to*/
    *popu_ptr = (population*) malloc(sizeof (population));
    if (NULL == popu_ptr)
        return -1;
    popu = *popu_ptr;

    /* Set fields */
    popu->polygon = poly;
    popu->numpoints = numpoints;
    popu->size = size;
    popu->num_lovers = ((int) (size * LOVER_PERCENT / 200)) *2; /* force even */

    /* Allocate list of individu's */
    popu->list = (individu*) malloc(sizeof (individu) * (size + popu->num_lovers));
    if (NULL == popu->list)
    {
        free(popu);
        return -2;
    }

    /* Allocate all space for points at once */
    popu->_allpoints = (point*) malloc(sizeof (point) * (size + popu->num_lovers) * numpoints);
    if (NULL == popu->list)
    {
        free(popu->list);
        free(popu);
        return -3;
    }

    popu->best = -1.0;
    /* Fill with random points */
    for (i = 0; i < size + popu->num_lovers; i++)
    {
        /*Generate random individu's and add them*/
        init_individu(popu, popu->list + i, popu->_allpoints + (i * numpoints));
    }

    return 0;
}

int init_individu(population* popu, individu* solution, point* point_ptr)
{
    solution->points = point_ptr;
    polygon_random_points(popu->numpoints, solution->points, popu->polygon);

    /* Calculate fitness */
    get_fitness(popu, solution);
    return 0;
}

void free_population(population** popu)
{
    /* And the memory lived free and happily ever after*/
    free((*popu)->_allpoints);
    free((*popu)->list);
    free(*popu);

    /* Set pointer to NULL to avoid dangling pointers*/
    *popu = NULL;
}

double get_fitness(population* population, individu* indi)
{
    int i, j;
    double result = 0.0;
    point* l = indi->points;
    for (i = 0; i < population->numpoints; i++)
    {
        for (j = 0; j < population->numpoints; j++)
        {
            /* add sqrt(euclidian distance)*/
            result += sqrt(sqrt((l[i].x - l[j].x)*(l[i].x - l[j].x)+(l[i].y - l[j].y)*(l[i].y - l[j].y)));
        }
    }
    if (result > population->best)
    {
        population->best = result;
    }
    indi->fitness = result;
    return result;
}

int get_best(population* population)
{
    int best = 0;
    int i;
    /* loop over all individus */
    for (i = 0; i < population->size; i++)
    {
        if (population->list[i].fitness > population->list[best].fitness)
            best = i;

    }
    return best;
}

int do_iterations(population* population, int num_generations)
{
    /* Allocate an array to store the indices of the individus that pair*/
    int *lovers_indices = (int*) malloc(population->num_lovers * sizeof (int));

    int i, num_kids;
    double best = population->list[get_best(population)].fitness; /* elite magic number for good luck */
    static double best_diff = 50000;
    double prev_best = best;
    if (NULL == lovers_indices)
        return -1;

    /* In debug print a table of the progress */
    log_dbg("|  GEN  | FITNESS |     DELTA   | ACCU WEIGHT |\n");

    /* Do num_generations times */
    for (i = 0; i < num_generations; i++)
    {
        /* Make kids and add them to the array (index in [size,size+lovers])*/
        num_kids = do_sex(population, lovers_indices);

        /* Kill individus to keep population size fixed*/
        do_deathmatch(population, num_kids);


        /* Get best fitness */
        best = population->best;
        /* In debug print a table of the progress */
        best_diff = best_diff * (double) WEIGHTING_DECREASE
                + (((double) 1 - (double) WEIGHTING_DECREASE) * fabs(best - prev_best));


        if(0==(i % 100))
            log_dbg("|  %-3d  | % 3.3f | %+3.8f | %3.8f |\n", i, best, best - prev_best, best_diff);

        prev_best = best;

        if (i > MIN_ITERATIONS  &&best_diff < MIN_PRECISION)
        {
            break;
        }

    }

    /* free the array of indices */
    free(lovers_indices);
    return i;
}

int do_sex(population* population, int* lovers_indices)
{
    int size = population->size;
    individu* list = population->list;
    int i, num_mates;

    /* fill lovers_indices with stochastically chosen mates */
    num_mates = do_mate_selection(population, lovers_indices);
    num_mates = (num_mates / 2)*2; /* make even */

    /* Loop over mates in pairs of 2 */
    for (i = 0; i < num_mates; i += 2)
    {
        /* Do crossover of the selected mates from the [0,size-1] part of the */
        /* population and store their childs in the [size, size + num_lovers] */
        /* part of the array                                                  */
        do_crossover(
                     population,
                     list + (lovers_indices[i]), list + (lovers_indices[i + 1]),
                     list + (size + i), list + (size + i + 1)
                     );

        /* Mutate kids ? */
        if (rand() % MUTATION_1_IN == 0)
            do_mutation(population, list + size + i);

        if (rand() % MUTATION_1_IN == 0)
            do_mutation(population, list + size + i + 1);

        /* Get fitness of childs and store in child->fitness  */
        get_fitness(population, list + size + i);
        get_fitness(population, list + size + i + 1);
    }


    return i;
}
#if SUS

/**
 * MATE SELECTION IF SUS
 */

int do_mate_selection(population* population, int* indices)
{
    int i = 0;
    double total_fitness = 0.0;
    double offset, interval, target, counter = 0;
    int curIndex = 0;

    /* Calculate the total fitness */
    for (i = 0; i < population->size; i++)
        total_fitness += population->list[i].fitness;

    /* set the interval size */
    interval = total_fitness / (double) population->num_lovers;

    /* Make random offset */
    offset = (double) rand() * interval / (double) RAND_MAX;

    /* start Stochastic universal sampling  */
    target = offset;
    for (i = 0; i < population->size; i++)
    {
        /* increment counter */
        counter += population->list[i].fitness;

        /* if reached target: add index to list and increment target*/
        if (counter >= target)
        {
            indices[curIndex] = i;
            curIndex++;
            if (curIndex >= population->num_lovers)
                break;
            target += interval;
        }

    }

    return curIndex;
}
#else

/**
 * MATE SELECTION IF NOSUS
 */
int do_mate_selection(population* plebs, int* indices)
{
    int population_size = plebs->size;
    int i;
    int group_size = population_size * SELECTION_PRESSURE / 100;

    /* Try to find a worse individu by selecting group_size random opponents */
    for (i = 0; i < plebs->num_lovers; i++)
    {
        indices[i] = do_pos_tournament(plebs,group_size);
    }

    return plebs->num_lovers;
}

#endif

#if RANDOM_CROSSOVER

int do_crossover(population* population, individu* papa, individu* mama, individu* son, individu* daughter)
{
    individu * parents[2] = {mama, papa};
    int i, r;

    /* first part */
    for (i = 0; i < population->numpoints; i++)
    {
        r = rand() % 2;
        son->points[i] = parents[r]->points[i];
        daughter->points[i] = parents[1 - r]->points[i];
    }
    return 0;
}

#else

int do_crossover(population* population, individu* papa, individu* mama, individu* son, individu* daughter)
{
    int split_index = 1 + (rand() % (population->numpoints - 1));
    int i;

    /* first part */
    for (i = 0; i < split_index; i++)
    {
        son->points[i] = papa->points[i];
        daughter->points[i] = mama->points[i];
    }
    /* continue with second part*/
    for (i = i; i < population->numpoints; i++)
    {
        son->points[i] = mama->points[i];
        daughter->points[i] = papa->points[i];
    }
    return 0;
}
#endif

int do_mutation(population* population, individu* individu)
{
    /* get a random point */
    int randindex = rand() % population->numpoints;

    /* store start values*/
    float base_x = individu->points[randindex].x;
    float base_y = individu->points[randindex].y;

    float new_x, new_y;

    /* maximum change is a fraction of the maximum distance in the polygon */
    float max_delta = population->polygon->diagonal / (float) MUTATION_DELTA;


    /* Add/subtract random small values from the point; */
    /* reduce max if result out of polygon */
    do
    {
        max_delta = max_delta / 2.0;
        if (max_delta < FLT_MIN)
        {
            log_dbg("[warn] FAILED to mutate\n");
            return -1;
        }

        /* generate new coordinated*/
        new_x = base_x + max_delta * ((double) rand() / (double) RAND_MAX)*(rand() % 2 ? 1.0 : -1.0);
        new_y = base_y + max_delta * ((double) rand() / (double) RAND_MAX)*(rand() % 2 ? 1.0 : -1.0);
    }
    while (!polygon_contains(new_x, new_y, population->polygon));

    /* set new coordinates */
    individu->points[randindex].x = new_x;
    individu->points[randindex].y = new_y;

    return 0;
}

int do_deathmatch(population* plebs, int to_kill)
{
    int left, victim, i;

    /* Group size for tournament selection:     */
    /* SELECTION_PRESSURE percent of the total  */
    int group_size = (plebs->size + to_kill) * SELECTION_PRESSURE / 100;

    /* to_kill times*/
    for (left = to_kill; left >= 0; left--)
    {
        /* Get the id of an individu to remove*/
        victim = do_neg_tournament(plebs, group_size, left);

        /* fill hole created by killing victim with a new child    */
        /* plebs[victim] to be replaced by  plebs[size + left - 1] */
        plebs->list[victim].fitness = plebs->list[plebs->size + left - 1].fitness;

        for (i = 0; i < plebs->numpoints; i++)
        {
            plebs->list[victim].points[i] = plebs->list[plebs->size + left - 1].points[i];
        }
    }

    return left;
}

int do_neg_tournament(population* plebs, int group_size, int excess_num)
{
    int population_size = plebs->size + excess_num;
    /* Select a random individu as current worst*/
    int worst_ID = rand() % population_size;
    int i, cur_id;

    /* Try to find a worse individu by selecting group_size random opponents */
    for (i = 0; i < group_size; i++)
    {
        cur_id = rand() % population_size;
        if (plebs->list[cur_id].fitness < plebs->list[worst_ID].fitness)
            worst_ID = cur_id;
    }

    return worst_ID;
}

int do_pos_tournament(population* plebs, int group_size){
        int best_ID = rand() % plebs->size;
        int cur_id,j;
        for (j = 1; j < group_size; j++)
        {
            cur_id = rand() % plebs->size;
            if (plebs->list[cur_id].fitness > plebs->list[best_ID].fitness)
                best_ID = cur_id;
        }
        return best_ID;
}

void population_print(population* population)
{
    int i;
    printf("Printing population of size %d\n", population->size);

    for (i = 0; i < population->size; i++)
    {
        get_fitness(population, population->list + i);
        individu_print(population, population->list + i);
    }
}

void individu_print(population* population, individu* individu)
{
    int i;
    printf("Individu %p: %f\n", individu, individu->fitness);
    for (i = 0; i < population->numpoints; i++)
    {
        printf("\t%f\t%f\n", individu->points[i].x, individu->points[i].y);
    }

}



