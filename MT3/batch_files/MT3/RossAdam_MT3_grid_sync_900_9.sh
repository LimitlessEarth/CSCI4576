#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT3_grid_sync_9_1000.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=9
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT2 -s 1000 -d 2 -i input_files/conways_input.pgm -n 900