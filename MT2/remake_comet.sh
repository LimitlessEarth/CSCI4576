#!/bin/bash

git pull && make clean && make && cp RossAdam_MT2 ../../bin && rm /oasis/scratch/comet/adamross/temp_project/* && sbatch batch_files/RossAdam_testing.sh && watch squeue -u adamross && cat "dev/comet_out/$(ls -1rt dev/comet_out/ | tail -n1)" 