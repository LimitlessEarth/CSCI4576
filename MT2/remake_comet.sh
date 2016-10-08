#!/bin/bash

git pull && make clean && make && cp RossAdam_MT2 ../../bin && sbatch batch_files/RossAdam_testing.sh && watch -d 5 squeue -u adamross