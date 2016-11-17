#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="hw9.1-adro4510-openmp-manual.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

./hw9.1-adro4510-openmp-manual