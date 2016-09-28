#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_serial_HW6.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2  
#SBATCH --export=ALL  
#SBATCH -t 00:05:00

ibrun RossAdam_serial_HW6