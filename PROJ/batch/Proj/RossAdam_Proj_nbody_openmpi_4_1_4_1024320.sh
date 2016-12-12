#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmpi_4_1_4_1024320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL  
#SBATCH -t 02:00:00


ibrun  nbody_openmpi -i 10 -p 1024320