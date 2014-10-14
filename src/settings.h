/* 
 * File:   settings.h
 * Author: Robbert Gurdeep Singh
 *
 * Created on 08 October 2014, 23:20
 */

#ifndef SETTINGS_H
#define	SETTINGS_H


/**
 * Minimum number of iterations 
 * Type: INT 
 * Default: 200
 */
#ifndef MIN_ITERATIONS
#define MIN_ITERATIONS  -1
#endif


/**
 * Maximum number of iterations 
 * Type: INT 
 * Default: 2000
 */
#ifndef MAX_INTERATIONS
#define MAX_INTERATIONS 50000
#endif


/**
 * If the exponentially weighted moving average of the difference of the 
 * average fitness is below this, the program will consider the answer good 
 * enough, and will stop. 
 * Type: Positive number
 * Default: 0.001
 */
#ifndef MIN_PRECISION
#define MIN_PRECISION  0.00000000001
#endif


/**
 * Amount that the previous average counts in the  exponentially weighted moving 
 * average.
 * Type: DOUBLE between 0 and 1 (preveribly below 0.5)
 * Default: 0.45
 */
#ifndef WEIGHTING_DECREASE
#define WEIGHTING_DECREASE 0.45
#endif


/**
 * Number of individu's in the population
 * Type: INT
 * Default: 1500;
 */
#ifndef NUM_INDIVIDUS
#define NUM_INDIVIDUS 256
#endif


/**
 * Percent of the population that will take part in mating each iteration
 * Type: INT between 0 and 100
 * Default: 10
 */
#ifndef LOVER_PERCENT
#define LOVER_PERCENT 10
#endif


/**
 * The chance that a child will mutate
 * Type: INT positive
 * Default: 3
 */
#ifndef MUTATION_1_IN
#define MUTATION_1_IN 3
#endif


/**
 * The size of the mutation
 * Type: DOUBLE
 * Default: 5.0
 */
#ifndef MUTATION_DELTA
#define MUTATION_DELTA 5.0
#endif


/*
 * Likelyhood that only the best individus will keep living.
 * Type: INT greater than 0
 * Default:  50
 */
#ifndef SELECTION_PRESSURE
#define SELECTION_PRESSURE 50
#endif


#endif	/* SETTINGS_H */

