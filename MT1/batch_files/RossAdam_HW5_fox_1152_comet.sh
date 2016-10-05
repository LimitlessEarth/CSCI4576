#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT1_test.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=9 
#SBATCH --export=ALL  
#SBATCH -t 00:05:00

ibrun RossAdam_MT1 -d 1 -i input_files/conways_input.pgm -n 1000 -c 500