/* 
 * File:   main.c
 * Author: Robbert Gurdeep Singh
 *
 * Created on 30 September 2014, 13:48
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <time.h>  

#include "polygon.h"
#include "genetic_base.h"
#include "settings.h"

#ifdef TESTINIT

void test() {
    polygon poly = {0};
    printf("Testing polygon read and contains\n\n");
    assert(polygon_read("../tests/soos.poly", &poly) == 0);
    assert(polygon_contains(1.236, 0.719, &poly) == 0);
    assert(polygon_contains(3.000, 0.605, &poly) == 0);
    assert(polygon_contains(4.628, 1.156, &poly) == 0);
    assert(polygon_contains(4.772, 3.799, &poly) == 0);
    assert(polygon_contains(1.444, 3.606, &poly) == 0);
    assert(polygon_contains(1.684, 1.681, &poly) == 1);
    assert(polygon_contains(3.000, 1.848, &poly) == 1);
    assert(polygon_contains(2.260, 1.751, &poly) == 1);
    polygon_print(&poly);

}
#endif


#ifdef TESTRAND

#define TESTRANDNUM 500
void test() {
    polygon poly = {0};
    int i;
    point stuff[TESTRANDNUM] = {0};
    assert(polygon_read("../tests/soos.poly", &poly) == 0);
    assert(polygon_random_points(TESTRANDNUM, stuff, &poly) == 0);


    printf("1337\n");
    for (i = 0; i < TESTRANDNUM; i++) {
        printf("%f %f\n", stuff[i].x, stuff[i].y);

    }
}
#undef TESTRANDNUM
#endif


#ifdef TESPOPULATOR

void test() {
    polygon poly = {0};
    population popu = {0};
    assert(polygon_read("../tests/soos.poly", &poly) == 0);
    init_population(5,&poly,&popu);
    population_print(&popu);
    
    do_sex(&popu);
    
    free_population(&popu);
    polygon_free(&poly);
}
#endif



int main(int argc, char** argv) {
    srand(time(NULL));
#if TESTINIT || TESTRAND || TESPOPULATOR
    test();
    return 0;
#else


    polygon poly = {0};

    printf("I've got %d tickets to paradise\n", argc);
    if (argc != 3) {
        printf("USAGE: %s num file\n", argv[0]);
        printf("num \t Number of points to add \n"
                "file \t A file containing data\n\n");
        polygon_read("../tests/soos.poly", &poly);
    } else {
        polygon_read(argv[2], &poly);
    }

    polygon_print(&poly);
#endif
}