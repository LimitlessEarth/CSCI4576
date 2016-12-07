CC = mpicc
NVCC = nvcc
CCFLAGS = -g -Wall -std=c99
NVCCFLAGS = -arch=sm_35 -ftz=true -I../
LDFLAGS = -fopenmp
ifeq ($(DEBUG),on)
	CCFLAGS += -DDEBUG
endif

C_FILES_SERIAL = nbody_serial.c pprintf.c helper.c
O_FILES_SERIAL = nbody_serial.o pprintf.o helper.o

C_FILES_OPENMP = nbody_openmp.c pprintf.c helper.c
O_FILES_OPENMP = nbody_openmp.o pprintf.o helper.o

C_FILES_OPENMPI = nbody_openmpi.c pprintf.c helper.c
O_FILES_OPENMPI = nbody_openmpi.o pprintf.o helper.o

C_FILES_OPENMP_MPI = nbody_openmp_mpi.c pprintf.c helper.c
O_FILES_OPENMP_MPI = nbody_openmp_mpi.o pprintf.o helper.o

C_FILES_CUDA = nbody_cuda.cu

all: Serial Openmp Openmpi Openmpmpi
cuda: Cuda

Serial: $(O_FILES_SERIAL)
	$(CC) -o nbody_serial $(O_FILES_SERIAL) $(LDFLAGS)

Openmp: $(O_FILES_OPENMP)
	$(CC) -o nbody_openmp $(O_FILES_OPENMP) $(LDFLAGS)

Openmpi: $(O_FILES_OPENMPI)
	$(CC) -o nbody_openmpi $(O_FILES_OPENMPI) $(LDFLAGS)
    
Openmpmpi: $(O_FILES_OPENMP_MPI)
	$(CC) -o nbody_openmp_mpi $(O_FILES_OPENMP_MPI) $(LDFLAGS)

Cuda: $(O_FILES_CUDA)
	$(NVCC) $(NVCCFLAGS) -o nbody_cuda $(C_FILES_CUDA)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES_SERIAL) nbody_serial
	/bin/rm -f core $(O_FILES_OPENMP) nbody_openmp
	/bin/rm -f core $(O_FILES_OPENMPI) nbody_openmpi
	/bin/rm -f core $(O_FILES_OPENMP_MPI) nbody_openmp_mpi
	/bin/rm -f nbody_cuda
	/bin/rm -f nbody_openmp_var.o openmp_block/*
    
install:
	if test -d ../../bin; then \
		cp nbody_serial ../../bin; \
		cp nbody_openmp ../../bin; \
		cp nbody_openmpi ../../bin; \
		cp nbody_openmp_mpi ../../bin; \
    fi 

    

Serial: pprintf.o helper.o
Openmp: pprintf.o helper.o
Openmpi: pprintf.o helper.o
Openmpmpi: pprintf.o helper.o
Cuda: pprintf.o helper.o

.c.o:		
	$(CC) $(CCFLAGS) $(LDFLAGS) -c -o $*.o $*.c
    
# All of the object files depend on the globals, so rebuild everything if they
# change!
*.o: globals.h 

# Nothing really depends on the pprintf prototypes, but just be safe
*.o: pprintf.h

*.o: helper.h
