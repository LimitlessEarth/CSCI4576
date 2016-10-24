#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT3_row_async_25_2700.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=5
#SBATCH --ntasks-per-node=5
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT2 -a -s 2700 -d 1 -i input_files/conways_input.pgm -n 500