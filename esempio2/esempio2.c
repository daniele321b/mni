#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char **argv) {
     int me, nproc; 
     MPI_Status status;
     double a[2]={0,0}, b[2]={0,0};

 
       
      MPI_Init (&argc, &argv); 
      MPI_Comm_size (MPI_COMM_WORLD,&nproc); 
      MPI_Comm_rank (MPI_COMM_WORLD, &me); 

     
      if (me == 0) {
           a[0]=3.0, a[1]=5.0;
           MPI_Send(&a[0],2,MPI_DOUBLE,1,10,MPI_COMM_WORLD);
          
      }
      else if (me==1) {
           MPI_Recv(&b[0],2,MPI_DOUBLE,0,10,MPI_COMM_WORLD,&status);
           printf ("%d: a[0]=%lf a[1]=%lf\n",me,a[0],a[1]);
           printf ("%d: b[0]=%lf b[1]=%lf\n",me,b[0],b[1]);
      }
      
   
      MPI_Finalize (); /* Disattiva MPI */
      
    
      //system("PAUSE");
      return 0;
} 