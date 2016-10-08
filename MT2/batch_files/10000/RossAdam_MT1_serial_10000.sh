#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT1_serial_10000.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1 
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT1 -d 0 -i input_files/conways_input.pgm -n 10000 -c 999