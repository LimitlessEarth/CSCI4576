#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_nbody_serial_1_1_1_128160.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --export=ALL  
#SBATCH -t 00:45:00


ibrun  nbody_serial -i 20 -p 128160