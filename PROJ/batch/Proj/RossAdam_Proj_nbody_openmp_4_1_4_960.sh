#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_4_1_4_960.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL  
#SBATCH -t 00:10:00

export OMP_NUM_THREADS=4
ibrun --npernode 1 nbody_openmp -i 60 -p 960