SRC=nbody_cuda.cu
EXE=nbody_cuda

nvcc -arch=sm_35 -ftz=true -I../ -o $EXE $SRC

echo $EXE
