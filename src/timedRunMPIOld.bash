#!/bin/bash
#
# File:   timedRunMPI.bash
# Author: garonn
#
# Created on 15-Nov-2014, 21:51:20
#
/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mpirun -np $1 ./maxdist_mpi_old  $2 "$3"
