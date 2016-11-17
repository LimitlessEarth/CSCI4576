#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="serial.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

./serial