#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

/**
* Funzione che esegue il prodotto matrice vettore
*/
void prod_mat_vett(int w[], int *a, int ROWS, int COLS, int v[])
{
    int i, j;
    //Stampa

    for (i = 0; i < ROWS; i++)
    {
        //w[i] = 0;
        for (j = 0; j < COLS; j++)
        {
            w[i] += a[i * COLS + j] * v[j];
            //printf(" %lf * %lf = %lf \n", a[i * COLS + j], v[j], w[i]);
        }
    }
}

void trasp_mat(int *a, int *b, int m, int n)
{

    int i, j;
    //printf("Trasposta \n \n");
    for (i = 0; i < m; i++) //righe
    {
        for (j = 0; j < n; j++) //colonne
        {
            b[j * n + i] = a[i * n + j];
        }
        //printf("\n");
    }
    //printf("\n");
}

int main(int argc, char **argv)
{

    int nproc; // Numero di processi totale
    int me;    // Il mio id
    int m, n;
    int local_n;
    int *A, *B, *v, *w;
    /*Attiva MPI*/
    MPI_Init(&argc, &argv);
    /*Trova il numero totale dei processi*/
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    /*Da ad ogni processo il proprio numero identificativo*/
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    if (me == 0)
    {

        if (argc < 3)
        {
            printf("inserire numero di righe m = \n");
            fflush(stdout);
            scanf("%d", &m);
            printf("inserire numero di colonne n = \n");
            fflush(stdout);
            scanf("%d", &n);
        }
        else
        {
            m = atoi(argv[1]);
            n = atoi(argv[2]);
        }

        // Alloco spazio di memoria
        A = malloc(m * n * sizeof(int));
        B = malloc(m * n * sizeof(int));
        v = malloc(sizeof(int) * n);
        w = malloc(sizeof(int) * n);

        local_n = n / nproc;
    }

    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    trasp_mat(A, B, m, n);

    //Inizio a lavorare su B
    int *localB = malloc(m * local_n * (sizeof(int)));
    MPI_Scatter(&B[0], local_n * m, MPI_INT, &localB[0], local_n * m, MPI_INT, 0, MPI_COMM_WORLD);

    // int *localv = malloc(local_n * sizeof(int));
    // MPI_Scatter(&v[0], local_n, MPI_INT, &localv, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // int *localA = malloc(m * local_n * (sizeof(int)));
    // trasp_mat(localB, localA, local_n, m);

    // int *local_w = malloc(local_n * sizeof(int));
    // prod_mat_vett(local_w, localA, m, local_n, localv);

    // MPI_Reduce(&local_w[0], &w[0], m, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // if (me == 0)
    // {
    //     printf("RIsualtati \n");
    //     printf("w = \n");
    //     for (int i = 0; i < n; i++)
    //         printf("%d ", w[i]);
    //     printf("\n");
    // }
    MPI_Finalize(); /* Disattiva MPI */
    return 0;
}