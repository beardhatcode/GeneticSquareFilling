#!/bin/bash
#PBS -N mpiworktest ## job name
#PBS -q default ## default queue (~ requested resources)
#PBS -l nodes=2:ppn=all ## 2-node job
#PBS -l walltime=10:00:00 ## max. 10 hour of wall time
#PBS -l vmem=4gb ## max. 4GB virtual memory

module load foss/2014b
module load scripts

cd $PBS_O_WORKDIR

(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun maxdist_mpi    10 vierkant.poly ) &>> norm
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun  maxdist_mpi   500 vierkant.poly ) &>> norm 
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun maxdist_mpi_coppy    10 vierkant.poly ) &>> coppy
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun  maxdist_mpi_coppy   500 vierkant.poly ) &>> coppy


(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  1 maxdist_mpi   500 vierkant.poly ) &>> norm
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  2 maxdist_mpi   500 vierkant.poly ) &>> norm 
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  4 maxdist_mpi   500 vierkant.poly ) &>> norm 
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  8 maxdist_mpi   500 vierkant.poly ) &>> norm 
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h 16 maxdist_mpi   500 vierkant.poly ) &>> norm 


(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  1 maxdist_mpi_coppy   500 vierkant.poly ) &>> coppy
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  2 maxdist_mpi_coppy   500 vierkant.poly ) &>> coppy
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  4 maxdist_mpi_coppy   500 vierkant.poly ) &>> coppy
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h  8 maxdist_mpi_coppy   500 vierkant.poly ) &>> coppy
(/usr/bin/time -f "%C\n%e real\n%U user\n%S sys\n" mympirun -h 16 maxdist_mpi_coppy   500 vierkant.poly ) &>> coppy

