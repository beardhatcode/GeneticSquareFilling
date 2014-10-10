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

    /* Fill with random points */
    for (i = 0; i < size + popu->num_lovers; i++)
    {
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
    indi->fitness = result;
    return result;
}

int get_best(population* population)
{
    int best = 0;
    int i;
    /* loop over all individus */
    for (i = 1; i < population->size; i++)
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

    int i, j, num_kids;
    double avg;
    double prev_avg = 0.0;

    if (NULL == lovers_indices)
        return -1;

    /* In debug print a table of the progress */
    log_dbg("|  GEN  | FITNESS |   DELTA  |\n");

    /* Do num_generations times */
    for (i = 0; i < num_generations; i++)
    {
        /* Make kids and add them to the array (index in [size,size+lovers])*/
        num_kids = do_sex(population, lovers_indices);
       
        /* Kill individus to keep population size fixed*/
        do_deathmatch(population, num_kids);


        /* Get average fitness */
        /* note: compiler will remove this if we're not debugging */
        avg = 0.0;
        for (j = 0; j < population->size; j++)
        {
            avg += population->list[i].fitness;
        }
        avg = avg / (double) population->size;

        /* In debug print a table of the progress */
        log_dbg("|  %-3d  | % 3.3f | %+3.3f |\n", i, avg, avg - prev_avg);
        prev_avg = avg;

    }

    /* free the array of indices */
    free(lovers_indices);
    return 0;
}

int do_sex(population* population, int* lovers_indices)
{
    int size = population->size;
    individu* list = population->list;
    int i, num_mates;

    /* fill lovers_indices with stochastically chosen mates */
    num_mates = do_mate_selection(population, lovers_indices);
    num_mates = (num_mates / 2 )*2; /* make even */
    
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

int do_mate_selection(population* population, int* indices)
{
    int i = 0;
    double total_fitness = 0.0;
    double offset,interval,target,counter;
    int curIndex = 0;

    /* Calculate the total fitness */
    for (i = 0; i < population->size; i++)
        total_fitness += get_fitness(population, population->list + i);

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

int do_mutation(population* population, individu* individu)
{
    int randindex = rand() % population->numpoints;
    float baseX = individu->points[randindex].x;
    float baseY = individu->points[randindex].y;
    float new_x, new_y;
    float max_delta = population->polygon->diagonal / (float) MUTATION_DELTA;
    do
    {
        max_delta = max_delta / 2.0;
        if (max_delta < FLT_MIN)
        {
            log_dbg("FAILED to mutate\n");
            return -1;
        }
        new_x = baseX + max_delta * ((double) rand() / (double) RAND_MAX)*(rand() % 2 ? 1.0 : -1.0);
        new_y = baseY + max_delta * ((double) rand() / (double) RAND_MAX)*(rand() % 2 ? 1.0 : -1.0);
    }
    while (!polygon_contains(new_x, new_y, population->polygon));

    individu->points[randindex].x = new_x;
    individu->points[randindex].y = new_y;

    return 0;
}

int do_deathmatch(population* plebs, int to_kill)
{
    int left, victim, i;
    int group_size = (plebs->size + plebs->num_lovers) * 100 / SELECTION_PRESSUERE;
    for (left = to_kill; left >= 0; left--)
    {
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
        for (i = 0; i < plebs->numpoints; i++)
        {
            plebs->list[victim].points[i] = plebs->list[plebs->size + left - 1].points[i];
        }

        //individu_print(plebs->list + victim);
        //individu_print(plebs->list + plebs->size + left - 1);

        //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n %d left ~~~~",left);
    }

    return left;
}

int do_tournament_selection(population* plebs, int group_size, int over_size)
{
    int population_size = plebs->size + over_size;
    int worst_ID = rand() % population_size;
    int i, cur_id;
    for (i = 0; i < group_size; i++)
    {
        cur_id = rand() % population_size;
        if (plebs->list[cur_id].fitness < plebs->list[worst_ID].fitness)
            worst_ID = cur_id;
    }

    return worst_ID;
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



