#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT1_async_9_1000.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=9
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT1 -d 1 -s -i input_files/conways_input.pgm -n 1000 -c 1000