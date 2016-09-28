#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_fox_1152_HW6.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=9
#SBATCH --ntasks-per-node=16  
#SBATCH --export=ALL  
#SBATCH -t 00:05:00

ibrun RossAdam_fox_144_HW6