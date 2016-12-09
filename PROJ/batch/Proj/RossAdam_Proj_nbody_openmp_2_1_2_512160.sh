#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmp_2_1_2_512160.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --export=ALL  
#SBATCH -t 01:30:00

export OMP_NUM_THREADS=2
ibrun --npernode 1 nbody_openmp -i 10 -p 512160