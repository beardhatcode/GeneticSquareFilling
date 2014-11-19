/* 
 * File:   main.c
 * Author: Robbert Gurdeep Singh
 *
 * Created on 30 September 2014, 13:48
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>  
#include "debugmacro.h"

#include "polygon.h"
#include "genetic_base.h"
#include "settings.h"

/******* TEST ** INITIATION **************/
#ifdef TESTINIT

void test()
{
    polygon poly = {0};
    polygon poly2 = {0};
    printf("Testing polygon read and contains\n\n");
    assert(polygon_read("../tests/soos.poly", &poly) == 0);
    assert(polygon_contains(1.236, 0.719, &poly) == 0);
    assert(polygon_contains(3.000, 0.605, &poly) == 0);
    assert(polygon_contains(4.628, 1.156, &poly) == 0);
    assert(polygon_contains(4.772, 3.799, &poly) == 0);
    assert(polygon_contains(1.444, 3.606, &poly) == 0);
    assert(polygon_contains(4.994639, 0.934278, &poly) == 0);
    assert(polygon_contains(1.684, 1.681, &poly) == 1);
    assert(polygon_contains(3.000, 1.848, &poly) == 1);
    assert(polygon_contains(2.260, 1.751, &poly) == 1);
    assert(polygon_read("../tests/vierkant.poly", &poly2) == 0);
    assert(polygon_contains(5.000000, -0.0004491, &poly2) == 0);
    assert(polygon_contains(0.000000, -0.0004491, &poly2) == 0);
    assert(polygon_contains(5.000000, 15.0004491, &poly2) == 0);
    assert(polygon_contains(2.500000, 2.5004491, &poly2) == 1);

    //printf("%s", polygon_contains(4.994639, 0.934278, &poly2) ? "T" : "F");
    polygon_print(&poly);
    
    polygon_free(&poly);
    polygon_free(&poly2);

}
#endif
/*****************************************/

#ifdef TESTRAND

#define TESTRANDNUM 500

void test()
{
    polygon poly = {0};
    int i;
    point stuff[TESTRANDNUM] = {0};
    assert(polygon_read("../tests/soos.poly", &poly) == 0);
    assert(polygon_random_points(TESTRANDNUM, stuff, &poly) == 0);


    printf("1337\n");
    for (i = 0; i < TESTRANDNUM; i++)
    {
        printf("%f %f\n", stuff[i].x, stuff[i].y);

    }
}
#undef TESTRANDNUM
#endif


#ifdef TESPOPULATOR

void test()
{
    polygon poly = {0};
    population* popu = NULL;
    int best, i;
    assert(polygon_read("../tests/soos.poly", &poly) == 0);

    init_population(NUM_INDIVIDUS, 8, &poly, &popu);
    //population_print(popu);

    do_iterations(popu, INTERATIONS);
    //population_print(popu);

    best = get_best(popu);

    printf("%f\n", popu->list[best].fitness);
    for (i = 0; i < popu->numpoints; i++)
    {
        printf("%f %f\n", popu->list[best].points[i].x, popu->list[best].points[i].y);

    }

    free_population(&popu);
    polygon_free(&poly);
}
#endif

int main(int argc, char** argv)
{
#if TESTINIT || TESTRAND || TESPOPULATOR
    /* run test method */
    srand(time(NULL));
    test();
    return 0;
#else
    /* normal running */
    int result = -1;
    polygon poly = {0};
    population* popu = NULL;
    int best, i;
    int numpoints;

    /* Good input */
    if (argc != 3) /* TODO: chek if number */
    {
        printf("USAGE: %s num file\n", argv[0]);
        printf("num \t Number of points to add \n"
               "file \t A file containing data\n\n");
        return -1;
    }


    result = sscanf(argv[1], "%d", &numpoints);
    if (result == 0)
    {
        printf("You must provide a number:\n");
        printf("You gave '%s'\n", argv[1]);
        return -1;
    }


    /* Seed the random generator */
    srand(time(NULL));

    /* reed arguments */
    numpoints = atoi(argv[1]);
    result = polygon_read(argv[2], &poly);
    if (result != 0)
    {
        fprintf(stderr, "Could not read/allocate polygon in %s!\n", argv[2]);
        return -1;
    }


    result = init_population(NUM_INDIVIDUS, numpoints, &poly, &popu);
    if (result != 0)
    {
        fprintf(stderr, "Could not allocate enough space!\n");
        return -2;
    }


    if (numpoints > 1)
    {
        result = do_iterations(popu, MAX_INTERATIONS);
        if (result < 0)
        {
            fprintf(stderr, "Failed iteration (code %d)!\n", result);
        }
    }
    else
    {
        /* one point, everything is optimal*/
        result = 1;
    }
    log_dbg("Finished iteration, %d iterations\n", result);

    /*get best*/
    best = get_best(popu);

#ifdef PERFORMANCE_PRINT
    printf("Iterations:\n%d\nFitness:\n%f\n", result, popu->list[best].fitness);
#else
    /* generate output*/
    printf("%f\n", popu->list[best].fitness);
    for (i = 0; i < popu->numpoints; i++)
    {
        printf("%f %f\n", popu->list[best].points[i].x, popu->list[best].points[i].y);
    }
#endif
    free_population(&popu);
    polygon_free(&poly);
#endif
}