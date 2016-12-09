cp nbody_serial_struct.c nbody_serial.c
make clean && make
sudo perf stat -e task-clock,page-faults,cycles,instructions,cache-references,cache-misses,L1-icache-misses,L1-dcache-misses ./nbody_serial -i 15 8000

cp nbody_serial_indiv.c nbody_serial.c
make clean && make
sudo perf stat -e task-clock,page-faults,cycles,instructions,cache-references,cache-misses,L1-icache-misses,L1-dcache-misses ./nbody_serial -i 15 8000

cp nbody_serial_unroll.c nbody_serial.c
make clean && make
sudo perf stat -e task-clock,page-faults,cycles,instructions,cache-references,cache-misses,L1-icache-misses,L1-dcache-misses ./nbody_serial -i 15 8000

cp nbody_serial_struct_unroll.c nbody_serial.c
make
sudo perf stat -e task-clock,page-faults,cycles,instructions,cache-references,cache-misses,L1-icache-misses,L1-dcache-misses ./nbody_serial -i 15 8000
