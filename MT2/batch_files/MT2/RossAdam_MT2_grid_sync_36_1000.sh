#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT1_grid_sync_26_1000.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT2 -d 2 -i input_files/conways_input.pgm -n 1000 -c 500