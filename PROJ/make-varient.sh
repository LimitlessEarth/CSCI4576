mpicc -g -Wall -std=c99 -fopenmp -lm -c -o pprintf.o pprintf.c
mpicc -g -Wall -std=c99 -fopenmp -lm -c -o helper.o helper.c

sed -i -e "s/REPLACE/$1/g" nbody_openmp_var.c

mpicc -g -Wall -std=c99 -fopenmp -lm -c -o nbody_openmp_var.o nbody_openmp_var.c
mpicc -o nbody_openmp_var nbody_openmp_var.o pprintf.o helper.o -fopenmp -lm

if [ ! -d openmp_block ]; then
    mkdir openmp_block
fi

mv nbody_openmp_var openmp_block/nbody_openmp$1
sed -i -e "s/$1/REPLACE/g" nbody_openmp_var.c

if [ -d ../../bin ]; then
    cp openmp_block/nbody_openmp$1 ../../bin/.
fi