Superclusters
==============
96 cores
1TB ram

250GB

25 november => deadline: down

module load foss/2014b

module swap cluster/raichu
gsub job.sh
  > returns job id
qstat
  > status


job ding:
```
#PBS -N solving_42             ##job name
#PBS -q default                ##default queu
#PBS -l node=2:ppn=all         ##2 node job
#PBS -l walltime=10:00:00      ##10 u walltime max


module load foss/2014b
cd $PBS_O_WORKDIR
mympirun ... ## geen params...
```
