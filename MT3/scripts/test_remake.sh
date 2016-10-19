#!/bin/bash 

git pull && rm MPIIOtest && mpicc -o MPIIOtest MPIIOtest.c && cp MPIIOtest ../../bin/. && sbatch batch_files/RossAdam_MPIIO_testing.sh && watch squeue -u adamross && cat "dev/comet_out/$(ls -1rt dev/comet_out/ | tail -n1)"