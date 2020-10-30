/* --------------------------------------------------------------------------
  SCOPO
  Calcolo della somma di n numeri interi
  in modo sequenziale su un singolo processore.
--------------------------------------------------------------------------
  DESCRIZIONE
  Il numero n di interi da sommare deve essere passato da linea di comando come:
  `./somma_seq 100000000` -> il porogramma eseguiraÌ€ la somma di 1 mln di interi
  (se il parametro non viene fornito il programma esegue la somma di 10 mila
elementi)
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define STD_N 100000

int main(int argc, char **argv)
{
  // Dichiamarazione delle variabili
  int n, sum;
  int *vett;

  MPI_Init(&argc, &argv);
  /* Inizializzazione delle variabili */
  sum = 0;
  // Controllo del parametro passato dalla riga di comando
  if (argc == 1)
  {
    printf("Inserire il numero di elementi da sommare: \n");
    fflush(stdout);
    scanf("%d", &n);
  }
  else
    n = atoi(argv[1]);
  // Inizializzazione della funzione rand
  srand((unsigned int)time(0));

  vett = (int *)calloc(n, sizeof(int));

  int i;
  printf("Vetore: ");
  for (i = 0; i < n; i++)
  {
    /*creazione del vettore contenente numeri casuali */
    //*(vett + i) = (int)rand() % 5 - 2;
    vett[i] = (int)rand() % 5 - 2;
    printf("%d ", vett[i]);
  }
  printf("\n");

  //clock_t begin = clock();  // Inizio del timer
  double begin = MPI_Wtime(); //inizio del cronometro per il calcolo del tempo di inizio
  for (i = 0; i < n; i++)
  {
    sum += *(vett + 1);
    ;
  }
  //clock_t end = clock();  // Fine del timer
  double end = MPI_Wtime() - begin; // calcolo del tempo di fine
  //double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  printf("SUM of %d random numbers: %d\n", n, sum);
  printf("TIME: %lf s\n", end);
  MPI_Finalize();
}