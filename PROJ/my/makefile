CC = mpicc
CCFLAGS = -g -Wall -std=c99
LDFLAGS = -fopenmp
ifeq ($(DEBUG),on)
	CCFLAGS += -DDEBUG
endif

C_FILES_SERIAL = serial_str.c
O_FILES_SERIAL = serial_str.o

C_FILES_OPENMP = openmp_str.c
O_FILES_OPENMP = openmp_str.o

all: Serial Openmp

Serial: $(O_FILES)
	$(CC) -o serial_str serial_str.c $(O_FILES) $(LDFLAGS)

Openmp: $(O_FILES)
	$(CC) -o openmp_str openmp_str.c $(O_FILES) $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES) serial_str
	/bin/rm -f core $(O_FILES) openmp_str

Serial: 

.c.o:		
	$(CC) $(CCFLAGS) -c -o $*.o $*.c


# All of the object files depend on the globals, so rebuild everything if they
# change!
*.o:  

# Nothing really depends on the pprintf prototypes, but just be safe
*.o: 

*.o: 
