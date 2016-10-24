#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_MT3_DISTTYPE_SYNCTYPE_NPNUM_WORLDSIZE.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=NODEVAR
#SBATCH --ntasks-per-node=TASKVAR
#SBATCH --export=ALL  
#SBATCH -t 00:15:00

ibrun RossAdam_MT2SYNCFLAG -s WORLDSIZE -d DISTNUM -i input_files/conways_input.pgm -n ITERNUM