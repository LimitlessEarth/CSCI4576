#include<stdio.h>
#include "mpi.h"

int main(int argc, char **argv){
    int i, rank, size, offset, nints, N=16;
    int buf[N];
    MPI_File out_file;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    for (i = 0; i < (N / size); i++){
        buf[i] = i + ((N / size) * rank);
    }
    
    offset = rank * (N / size) * sizeof(int);
    
    MPI_File_open(MPI_COMM_WORLD, "/oasis/scratch/comet/adamross/temp_project/datafile", MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &out_file);
    
    printf("\nRank: %d, Offset: %d\n", rank, offset);
    
    MPI_File_set_view(out_file, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    MPI_File_write(out_file, buf, (N/size), MPI_INT, &status);
    MPI_File_close(&out_file);
    
    MPI_Finalize();
    return 0;
}