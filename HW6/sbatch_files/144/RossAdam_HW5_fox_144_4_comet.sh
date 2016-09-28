#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_fox_144_4_HW6.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4  
#SBATCH --export=ALL  
#SBATCH -t 00:05:00

ibrun RossAdam_fox_144_HW6