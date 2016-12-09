#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmpi_12_1_12_128160.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 00:45:00


ibrun  nbody_openmpi -i 20 -p 128160