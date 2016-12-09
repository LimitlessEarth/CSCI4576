#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmpi_8_1_8_256320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --export=ALL  
#SBATCH -t 01:00:00


ibrun  nbody_openmpi -i 15 -p 256320