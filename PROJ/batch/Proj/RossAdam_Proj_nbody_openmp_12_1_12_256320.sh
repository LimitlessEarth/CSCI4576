#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_12_1_12_256320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 01:00:00

export OMP_NUM_THREADS=12
ibrun --npernode 1 nbody_openmp -i 15 -p 256320