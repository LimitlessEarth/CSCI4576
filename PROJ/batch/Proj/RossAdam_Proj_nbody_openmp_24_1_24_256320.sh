#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_24_1_24_256320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 01:00:00

export OMP_NUM_THREADS=24
ibrun --npernode 1 nbody_openmp -i 15 -p 256320