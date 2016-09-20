# Makefile

# Note: the variables CC, CFLAGS, LDFLAGS, etc., are usually left to be set
# outside of the makefile. Most system administrators set these variables so
# that compilations work "outside the box" with no developer intervention.
# Because we're compiling MPI programs using specific compilers and specific
# MPI libraries, they're redefined here for each system.

# Intel Compilers
CC = mpicc
CXX = mpicxx

CFLAGS = -Wall -std=c99 #-shared-intel
#CXXFLAGS = -DMPICH_IGNORE_CXX_SEEK
#LDFLAGS = -L/opt/mpich-xl64/lib -lmpich -q64

# C and Object Files
C_FILES = $(wildcard *.c)
#O_FILES = $(C_FILES:.c=.o)
#O_FILES = mpi_hello_world.o

# Main Targets
all: RossAdam_tcts_HW5 RossAdam_sm_HW5 RossAdam_transpose_HW5

RossAdam_tcts_HW5:
	$(CC) -o RossAdam_tcts_HW5 RossAdam_tcts_HW5.c $(LDFLAGS)

RossAdam_sm_HW5:
	$(CC) -o RossAdam_sm_HW5 RossAdam_sm_HW5.c $(LDFLAGS)

RossAdam_transpose_HW5:
	$(CC) -o RossAdam_transpose_HW5 RossAdam_transpose_HW5.c $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core RossAdam_tcts_HW5
	/bin/rm -f core RossAdam_sm_HW5
	/bin/rm -f core RossAdam_transpose_HW5
