/* 
 * File:   settings.h
 * Author: Robbert Gurdeep Singh
 *
 * Created on 08 October 2014, 23:20
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

/* 
 * Complile with -DO_ALT_SETTINGS
 * and other settings as -D...=... to change
 */
#ifndef O_ALT_SETTINGS 

/**
 * Minimum number of iterations 
 * Type: INT 
 * Default: 200
 */
#define MIN_ITERATIONS  1000

/**
 * Maximum number of iterations 
 * Type: INT 
 * Default: 2000
 */
#define MAX_INTERATIONS 2000

/**
 * If the exponentially weighted moving average of the diference of the 
 * average fitness is below this, the program will considder the answer good 
 * enough, and will stop. 
 * Type: Positive number
 * Default: 0.001
 */
#define MIN_PRECISION  0.001

/**
 * Amount that the previous average counts in the  exponentially weighted moving 
 * average.
 * Type: DOUBLE between 0 and 1
 * Default: 0.95
 */
#define WEIGHTING_DECREASE 0.95


/**
 * Number of individu's in the population
 * Type: INT
 * Default: 1500;
 */
#define NUM_INDIVIDUS 256

/**
 * Percent of the population that will take part in mating each iteration
 * Type: INT between 0 and 100
 * Default: 10
 */
#define LOVER_PERCENT 10

#define MUTATION_1_IN 3
#define MUTATION_DELTA 5.0

/*
 * Likelyhood that only the best individus will keep living.
 * Type: INT greater than 0
 * Default:  50
 */
#define SELECTION_PRESSURE 50

#endif
#endif	/* SETTINGS_H */

