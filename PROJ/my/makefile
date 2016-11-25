CC = mpicc
CCFLAGS = -g -Wall -std=c99
LDFLAGS = -fopenmp
ifeq ($(DEBUG),on)
	CCFLAGS += -DDEBUG
endif

C_FILES_SERIAL = serial.c
O_FILES_SERIAL = serial.o

C_FILES_OPENMP = openmp.c
O_FILES_OPENMP = openmp.o

all: Serial Openmp

Serial: $(O_FILES)
	$(CC) -o serial serial.c $(O_FILES) $(LDFLAGS)

Openmp: $(O_FILES)
	$(CC) -o openmp openmp.c $(O_FILES) $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES) serial
	/bin/rm -f core $(O_FILES) openmp

Serial: 

.c.o:		
	$(CC) $(CCFLAGS) -c -o $*.o $*.c


# All of the object files depend on the globals, so rebuild everything if they
# change!
*.o:  

# Nothing really depends on the pprintf prototypes, but just be safe
*.o: 

*.o: 
