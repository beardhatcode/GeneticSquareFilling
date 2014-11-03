#!/bin/bash

for test in {LOVER_PERCENT,MUTATION_DELTA,SELECTION_PRESSURE,MUTATION_1_IN,NUM_INDIVIDUS,WEIGHTING_DECREASE};
do
    for poly in {vierkant,icosagon,soos};
    do
        ../extractor.sh $test $poly,50;
        ../extractor.sh $test $poly,15;
        ../extractor.sh $test $poly,5;
    done;
done;
