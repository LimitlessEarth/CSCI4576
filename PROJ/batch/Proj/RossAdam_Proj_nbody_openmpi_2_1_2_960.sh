#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmpi_2_1_2_960.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --export=ALL  
#SBATCH -t 00:10:00


ibrun  nbody_openmpi -i 60 -p 960