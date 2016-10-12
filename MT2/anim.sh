#!/bin/bash

#convert -delay 5 -loop 0 $(ls /animation2/*.pgm | sort -n -t . -k 1) anim.gif
cp /oasis/scratch/comet/adamross/temp_project/* dev/data/. && for f in `ls -1 dev/data`; do xxd -p -c 16 ${f}; sleep 0.5; done