#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_12_1_12_960.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 00:10:00

export OMP_NUM_THREADS=12
ibrun --npernode 1 nbody_openmp -i 60 -p 960