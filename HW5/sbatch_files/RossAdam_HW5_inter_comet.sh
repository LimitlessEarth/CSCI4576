#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_tcts_HW5_inter_comet.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1  
#SBATCH --export=ALL  
#SBATCH -t 00:05:00

ibrun RossAdam_tcts_HW5