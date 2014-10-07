/* 
 * File:   genetic_base.h
 * Author: garonn
 *
 * Created on 06 October 2014, 22:00
 */

#ifndef GENETIC_BASE_H
#define	GENETIC_BASE_H

#include "polygon.h"

typedef struct _individu{
    float fitness;
    point* points;
    
} individu;

typedef struct _population{
    float best_fitness;
    individu* best;
    int size;
    individu* list;
} population;
//
//
//int init_population(int size, polygon* poly , population* population);
//
//int init_individu(int numpoints, individu* individu);
//
//int free_population(population* population);
//
///**
// * It frees Willy
// * @param willy
// * @return 
// */
//int free_individu(individu* willy);
//
//int get_fitness(individu* individu);
//
//int invalidate_fitness(individu* individu);
//
//int do_sex(population* population);
//
//int do_crossover(individu* papa, individu* mama);
//
//int do_mutation(individu* );


#endif	/* GENETIC_BASE_H */

