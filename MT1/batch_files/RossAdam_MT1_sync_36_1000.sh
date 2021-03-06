#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT1_sync_36_1000.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=12
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT1 -d 1 -i input_files/conways_input.pgm -n 1000 -c 999