#!/bin/bash

#SBATCH -J rossadam  
#SBATCH -o RossAdam_MT3_DISTTYPE_SYNCTYPE_NPNUM_WORLDSIZE.%j.%N.out
#SBATCH -e taccError.e%j
#SBATCH -p normal
#SBATCH -n NODEVAR
#SBATCH -n TASKVAR
#SBATCH -t 00:15:00

ibrun RossAdam_MT3SYNCFLAG -s WORLDSIZE -d DISTNUM -i input_files/conways_input.pgm -n ITERNUM