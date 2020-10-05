#include <stdio.h>
#include <mpi.h>
int main(int argc, char *argv[]){
    int me, nproc;
   
   MPI_Init(&argc,&argv);
   MPI_Comm_size (MPI_COMM_WORLD,&nproc); 
   MPI_Comm_rank (MPI_COMM_WORLD,&me);

   printf("Sono %d di %d\n",me,nproc);   
   
   MPI_Finalize();
   return 0;
}