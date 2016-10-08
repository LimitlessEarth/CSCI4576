#!/bin/bash  
#SBATCH --job-name="rossadam"  
#SBATCH --output="dev/comet_out/RossAdam_MPIIO_test.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun MPIIOtest