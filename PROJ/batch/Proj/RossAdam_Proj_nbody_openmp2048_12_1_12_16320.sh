#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp2048_12_1_12_16320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 00:20:00

export OMP_NUM_THREADS=12
ibrun --npernode 1 nbody_openmp2048 -i 35 -p 16320