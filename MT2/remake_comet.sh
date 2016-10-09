#!/bin/bash

git pull && make clean && make && cp RossAdam_MT2 ../../bin && sbatch batch_files/RossAdam_testing.sh && watch squeue -u adamross && cat "dev/comet_out/$(ls -1rt dev/comet_out/ | tail -n1)" && for f in '$(ls /animation2/*.pgm | sort -n -t . -k 1)'; do xxd -p -c 16 ${f}; sleep 0.5; done