#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmpi_24_1_24_8160.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 00:15:00


ibrun  nbody_openmpi -i 40 -p 8160