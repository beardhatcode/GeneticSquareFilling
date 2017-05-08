# Genetic algorithm for optimal placement of dots in a convex polygon

This project shows an implementation of a [Genetic algorithm](https://en.wikipedia.org/wiki/Genetic_algorithm) to place point in a polygon in a way that the sum of the distances between those points is minimal.

[Dutch paper explaining the algorithm](/extra/tex/verslag.pdf)



## Single core variant

[The single core variant](/src/main.c) does a regular genetic algorithm. A large set of random solutions is made and each round they are mutated, crossed and selected. The [paper](/extra/tex/verslag.pdf) shows the optimal size of the selection and mutation. It also describes various methods for selection. [settings.h](/src/settings.h) contains the optimal values.



## MPI variant

This program can run on a cluster. To this end there is a [MPI based implementation](/src/main_mpi.c). Here, each processing node will execute the single core variant. To simulate one gigantic pool of solutions the results share their results form time to time. This is done by moving solutions from one node to another increasing diversity in the new node.