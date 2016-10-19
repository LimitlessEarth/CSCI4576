CC = mpicc
CCFLAGS = -g -Wall -std=c99
ifeq ($(DEBUG),on)
	CCFLAGS += -DDEBUG
endif

C_FILES = RossAdam_MT3.c pgm.c pprintf.c helper.c
O_FILES = RossAdam_MT3.o pgm.o pprintf.o helper.o

all: RossAdam_MT3

RossAdam_MT3: $(O_FILES)
	$(CC) -o RossAdam_MT3 $(O_FILES) $(LDFLAGS)

.PHONY: clean
clean:		
	/bin/rm -f core $(O_FILES) RossAdam_MT3

RossAdam_MT3: pgm.o pprintf.o helper.o

.c.o:		
	$(CC) $(CCFLAGS) -c -o $*.o $*.c


# All of the object files depend on the globals, so rebuild everything if they
# change!
*.o: globals.h 

# Nothing really depends on the pprintf prototypes, but just be safe
*.o: pprintf.h

*.o: helper.h

# Conway depends on PGM utilities
RossAdam_MT3.o: pgm.h pprintf.h helper.h
