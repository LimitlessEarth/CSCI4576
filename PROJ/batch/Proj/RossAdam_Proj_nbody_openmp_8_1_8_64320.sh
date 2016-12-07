#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_8_1_8_64320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --export=ALL  
#SBATCH -t 00:30:00

export OMP_NUM_THREADS=8
ibrun --npernode 1 nbody_openmp -i 25 -p 64320