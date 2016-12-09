#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_2_1_2_128160.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --export=ALL  
#SBATCH -t 00:45:00

export OMP_NUM_THREADS=2
ibrun --npernode 1 nbody_openmp -i 20 -p 128160