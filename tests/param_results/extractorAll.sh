#!/bin/bash

for test in {LOVER_PERCENT,MUTATION_DELTA,SELECTION_PRESSURE,MUTATION_1_IN,NUM_INDIVIDUS};
do
    for poly in {vierkant,icosagon,vierkant};
    do
        ../extractor.sh $test $poly,50;
    done;
done;
