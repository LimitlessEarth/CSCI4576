#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT3_row_sync_25_9000.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=5
#SBATCH --ntasks-per-node=5
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT2 -s 9000 -d 1 -i input_files/conways_input.pgm -n 200