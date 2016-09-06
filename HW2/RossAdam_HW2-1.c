/* RossAdam_HW2-1.c
 *
 *  Written by Adam Ross
 * 
 * Send a message in wrapping fashion to rank + 1.
 *    Process 0 prints the messages received.
 *
 * Input: none.
 * Output: contents of messages received by each process.
 *
 * 
 */

#include <stdio.h>
#include <string.h>
#include "mpi.h"

main(int argc, char* argv[]) {
    int         my_rank;       /* rank of process      */
    int         p;             /* number of processes  */
    int         source;        /* rank of sender       */
    int         dest;          /* rank of receiver     */
    int         tag = 50;       /* tag for messages     */
    char        message[100];  /* storage for message  */
    MPI_Status  status;        /* return status for    */
                               /* receive              */

    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank  */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    /* Create message */
    sprintf(message, "Greetings from process %d!", my_rank);
    dest = (my_rank + 1) % p;
    source = (my_rank == 0 ? (p - 1) : (my_rank - 1));
    /* Use strlen+1 so that '\0' gets transmitted */
    MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
    printf("Rank is: %d, Source is: %d, Dest is %d, Message recieved is: %s\n", my_rank, source, dest, message);

    /* Shut down MPI */
    MPI_Finalize();
} /* main */
