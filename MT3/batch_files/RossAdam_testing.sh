#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="dev/comet_out/RossAdam_MT2test.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT3 -w -d 2 -i input_files/conways_input.pgm -n 50 -c 10