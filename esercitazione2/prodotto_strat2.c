#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

/**
* Funzione che esegue il prodotto matrice vettore
*/
void prod_mat_vett(double w[], double *a, int ROWS, int COLS, double v[])
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

void trasp_mat(double *a, double *b, int m, int n)
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

    int nproc;       // Numero di processi totale
    int me;          // Il mio id
    int m, n;        // Dimensione della matrice
    int local_n;     // Dimensione dei dati locali
    int i, j, block; // Iteratori vari

    // Variabili di lavoro
    double *A, *B, *v, *localA, *local_w, *w;

    /*Attiva MPI*/
    MPI_Init(&argc, &argv);
    /*Trova il numero totale dei processi*/
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    /*Da ad ogni processo il proprio numero identificativo*/
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    // Se sono a radice
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

        // Numero di righe da processare
        local_n = n / nproc;

        // Alloco spazio di memoria
        A = malloc(m * n * sizeof(double));
        v = malloc(sizeof(double) * n);
        w = malloc(sizeof(double) * n);

        //printf("v = \n");
        for (j = 0; j < n; j++)
        {
            v[j] = j;
            printf("%f ", v[j]);
        }
        printf("\n");

        printf("A = \n");
        for (i = 0; i < m; i++)
        {
            for (j = 0; j < n; j++)
            {
                if (j == 0)
                    A[i * n + j] = 1.0 / (i + 1) - 1;
                else
                    A[i * n + j] = 1.0 / (i + 1) - pow(1.0 / 2.0, j);
                printf("%f ", A[i * n + j]);
            }
            printf("\n");
        }

        B = malloc(m * n * sizeof(double));
        // //trasposta di A
        trasp_mat(A, B, m, n);

        // for (i = 0; i < m; i++)
        // {
        //     //w[i] = 0;
        //     for (j = 0; j < n; j++)
        //     {
        //         printf("%lf ", B[i * n + j]);
        //     }
        //     printf("\n");
        // }

    } // fine me==0

    // Spedisco m, n, local_m e v
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Se sono un figlio alloco v
    if (me != 0)
        v = malloc(sizeof(double) * n);

    //Invio solo parte del vettore in sottovettore
    block = n / nproc;
    double *local_v = malloc(block * m * sizeof(double));

    //invio parte del vettore
    MPI_Scatter(&v[0], block, MPI_DOUBLE, &local_v[0], block, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // tutti allocano A locale e il vettore dei risultati
    localA = malloc(local_n * m * sizeof(double));
    local_w = malloc(m * sizeof(double));

    // Adesso 0 invia a tutti un pezzo della matrice
    int num = local_n * m;
    double start = MPI_Wtime();

    //invio trasposta
    MPI_Scatter(
        &B[0], num, MPI_DOUBLE,
        &localA[0], num, MPI_DOUBLE,
        0, MPI_COMM_WORLD);

    double *localB = malloc(local_n * m * sizeof(double));
    trasp_mat(localA, localB, local_n, n);

    // Scriviamo la matrice locale ricevuta
    // printf("localA %d = \n", me);

    // Effettuiamo i calcoli

    // printf("local me %d \n", me);
    // for (i = 0; i < m; i++)
    // {
    //     for (j = 0; j < local_n; j++)
    //         printf("%lf\t", localB[i * n + j]);
    //     printf("\n");
    // }

    prod_mat_vett(local_w, localB, m, local_n, local_v);

    double time = MPI_Wtime() - start;

    // printf("local w = \n");
    // for (i = 0; i < m; i++)
    //     printf("%f ", local_w[i]);
    // printf("\n");

    // MPI_Reduce(
    //     &local_w[0],
    //     &w[0],
    //     m,
    //     MPI_DOUBLE,
    //     MPI_SUM,
    //     0,
    //     MPI_COMM_WORLD);

    MPI_Gather(&local_w[0], local_n, MPI_DOUBLE, &w[0], local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //0 effettua la somma dei risultati

    // 0 raccoglie i risultati parziali

    // 0 stampa la soluzione
    if (me == 0)
    {
        printf("w = \n");
        for (i = 0; i < n; i++)
            printf("%f ", w[i]);
        printf("\n");
    }

    // if (me == 0)
    //     printf("Tempo di esecuzione %lf s \n", time);

    MPI_Finalize(); /* Disattiva MPI */
    return 0;
}