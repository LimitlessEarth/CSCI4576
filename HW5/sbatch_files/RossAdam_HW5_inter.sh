#!/bin/bash
#SBATCH -J rossadam	           	# job name
#SBATCH -o RossAdam_tcts_HW5_inter.o%j  # output file name (%j expands to jobID)
#SBATCH -e taccError.e%j        	# error file
#SBATCH -n 2              		# total number of mpi tasks requested
#SBATCH -N 2				# number of nodes
#SBATCH -p normal        		# queue (partition) -- normal, development, etc.
#SBATCH -t 00:05:00        		# run time (hh:mm:ss) - 1.5 hours

ibrun RossAdam_tcts_HW5              	# run the MPI executable named a.out