#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_openmpi_48_2_24_480.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 00:5:00


ibrun  nbody_openmpi -i 100 -p 480