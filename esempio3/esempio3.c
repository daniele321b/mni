#include <stdio.h>
#include <mpi.h>

int main (int argc, char **argv){

    int me, a, right, left, nproc;
    MPI_Status status;

    MPI_Init (&argc, &argv); 
    MPI_Comm_size (MPI_COMM_WORLD,&nproc); 
    MPI_Comm_rank (MPI_COMM_WORLD, &me); 

    left=(me-1+nproc)%nproc;
    right=(me+1)%nproc;


    MPI_Send(&me,1,MPI_INT,right,150,MPI_COMM_WORLD);
    MPI_Recv(&a,1,MPI_INT,left,150,MPI_COMM_WORLD,&status);
    printf("io sono il processo %d ed ho ricevuto %d dal processo %d\n",me,a,left);

    MPI_Finalize();
    return 0;

} 