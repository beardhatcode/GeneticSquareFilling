#!/bin/bash
#PBS -N mpiworktest ## job name
#PBS -q default ## default queue (~ requested resources)
#PBS -l nodes=2:ppn=all ## 2-node job
#PBS -l walltime=00:05:00 ## max. 5 hour of wall time
#PBS -l vmem=4gb ## max. 4GB virtual memory

module load foss/2014b
module load scripts

cd $PBS_O_WORKDIR

(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  6 maxdist_mpi   500 vierkant.poly ) &>> norm
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h 10 maxdist_mpi   500 vierkant.poly ) &>> norm 
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h 12 maxdist_mpi   500 vierkant.poly ) &>> norm 
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h 14 maxdist_mpi   500 vierkant.poly ) &>> norm 

