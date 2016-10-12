#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="dev/comet_out/RossAdam_MT2test.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT2 -s -d 2 -i input_files/glider_gun.pgm -n 200 -c 50