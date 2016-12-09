#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_serial_1_1_1_256320.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --export=ALL  
#SBATCH -t 01:00:00


ibrun  nbody_serial -i 15 -p 256320