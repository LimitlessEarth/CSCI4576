#!/bin/bash

#SBATCH --job-name="rossadam"  
#SBATCH --output="RossAdam_Proj_RUNFILE_NPNUM_NODEVAR_TASKVAR.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=NODEVAR
#SBATCH --ntasks-per-node=TASKVAR
#SBATCH --export=ALL  
#SBATCH -t TIME:00

THREAD_STATEMENT
ibrun NP_PER_NODE RUNFILE -i NUMITER -p NUMPART