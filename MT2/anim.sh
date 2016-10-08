#!/bin/bash

#convert -delay 5 -loop 0 $(ls /animation2/*.pgm | sort -n -t . -k 1) anim.gif
for f in `ls -1`; do xxd -p -c 16 ${f}; sleep 0.5; done