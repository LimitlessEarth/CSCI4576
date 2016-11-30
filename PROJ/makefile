CC = mpicc
CCFLAGS = -g -Wall -std=c99
LDFLAGS = -fopenmp
ifeq ($(DEBUG),on)
	CCFLAGS += -DDEBUG
endif

C_FILES_SERIAL = serial.c pprintf.c helper.c
O_FILES_SERIAL = serial.o pprintf.o helper.o

C_FILES_OPENMP = openmp.c pprintf.c helper.c
O_FILES_OPENMP = openmp.o pprintf.o helper.o

C_FILES_OPENMP_MPI = openmp_mpi.c pprintf.c helper.c
O_FILES_OPENMP_MPI = openmp_mpi.o pprintf.o helper.o

all: Serial Openmp Openmpmpi

Serial: $(O_FILES_SERIAL)
	$(CC) -o serial $(O_FILES_SERIAL) $(LDFLAGS)

Openmp: $(O_FILES_OPENMP)
	$(CC) -o openmp $(O_FILES_OPENMP) $(LDFLAGS)
    
Openmpmpi: $(O_FILES_OPENMP_MPI)
	$(CC) -o openmp_mpi $(O_FILES_OPENMP_MPI) $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES_SERIAL) serial
	/bin/rm -f core $(O_FILES_OPENMP) openmp
	/bin/rm -f core $(O_FILES_OPENMP_MPI) openmp_mpi

Serial: pprintf.o helper.o
Openmp: pprintf.o helper.o
Openmpmpi: pprintf.o helper.o

.c.o:		
	$(CC) $(CCFLAGS) $(LDFLAGS) -c -o $*.o $*.c
    
# All of the object files depend on the globals, so rebuild everything if they
# change!
*.o: globals.h 

# Nothing really depends on the pprintf prototypes, but just be safe
*.o: pprintf.h

*.o: helper.h

Serial: pprintf.h helper.h 