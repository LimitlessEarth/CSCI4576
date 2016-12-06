CC = mpicc
NVCC = nvcc
CCFLAGS = -g -Wall -std=c99
NVCCFLAGS = -arch=sm_35 -ftz=true -I../
LDFLAGS = -fopenmp -lm
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

C_FILES_CUDA = nbody_cuda.cu pprintf.c helper.c
O_FILES_CUDA = pprintf.o helper.o

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
	$(NVCC) -o nbody_cuda $(O_FILES_CUDA) $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES_SERIAL) nbody_serial
	/bin/rm -f core $(O_FILES_OPENMP) nbody_openmp
	/bin/rm -f core $(O_FILES_OPENMPI) nbody_openmpi
	/bin/rm -f core $(O_FILES_OPENMP_MPI) nbody_openmp_mpi

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

Serial: pprintf.h helper.h 
