#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="dev/comet_out/RossAdam_MT2test.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT2 -d 1 -i input_files/glider.pgm -n 10 -c 9