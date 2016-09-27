/* All arrays are allocated in calling program */
void Parallel_matrix_vector_prod(LOCAL_MATRIX_T local_A, int n, double A[], double local_B[], int local_n) {
    
    /* local_m = m/p, local_n = n/p */
    int i, int j, int k;
    double dot_prod;
    
    for (k = 0; k < n; k++) {
        MPI_Allgather(local_x, local_n, MPI_FLOAT, global_x, local_n, MPI_FLOAT, MPI_COMM_WORLD);
        for (i = 0; i < local_m; i++) {
            C[i][j] = 0.0;
            for (j = 0; j < n; j++) {
                C[i][j] += A[i][j] * B[j][i]
            }
        }
    }
}