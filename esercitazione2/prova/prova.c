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

    for (i = 0; i < ROWS; i++)
    {
        w[i] = 0;
        for (j = 0; j < COLS; j++)
        {
            w[i] += a[i * COLS + j] * v[j];
        }
    }
}
void prod_matTra_vett(double w[], double *a, int ROWS, int COLS, int local_n, double v[])
{
    int i, j;

    for (i = 0; i < ROWS; i++)
    {
        w[i] = 0;
        for (j = 0; j < local_n; j++)
        {
            w[i] += a[i * local_n + j] * v[j];
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

void trasp_trasp(double *a, double *b, int m, int n, int local_n)
{

    int i, j;
    //printf("Trasposta \n \n");
    for (i = 0; i < m; i++) //righe
    {
        for (j = 0; j < local_n; j++) //colonne
        {
            b[i * local_n + j] = a[j * n + i];
        }
        //printf("\n");
    }
    //printf("\n");
}

void stampa_mat(double *a, int m, int n)
{
    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
            printf("%lf\t", a[i * n + j]);
        printf("\n");
    }
}

int main(int argc, char **argv)
{

    int nproc;       // Numero di processi totale
    int me;          // Il mio id
    int m, n;        // Dimensione della matrice
    int local_n = 0; // Dimensione dei dati locali
    int i, j;        // Iteratori vari

    // Variabili di lavoro
    double *A, *AT, *v, *w, *localA, *localAT, *local_v, *local_w;
    double T_max;
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
        AT = malloc(m * n * sizeof(double));
        v = malloc(sizeof(double) * n);
        w = malloc(sizeof(double) * m);

        for (j = 0; j < n; j++)
        {
            v[j] = j;
        }

        for (i = 0; i < m; i++)
        {
            for (j = 0; j < n; j++)
            {
                if (j == 0)
                    A[i * n + j] = 1.0 / (i + 1) - 1;
                else
                    A[i * n + j] = 1.0 / (i + 1) - pow(1.0 / 2.0, j);
            }
        }
        //printf("Matrice A\n");
        //stampa_mat(A, m, n);
        trasp_mat(A, AT, m, n);
        //printf("Matrice AT\n");
        // stampa_mat(AT, m, n);

    } // fine me==0

    // Spedisco m, n, local_m e v
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Se sono un figlio alloco v
    if (me != 0)
        v = malloc(sizeof(double) * n);

    // tutti allocano A locale e il vettore dei risultati
    localA = malloc(local_n * n * sizeof(double));
    localAT = malloc(local_n * n * sizeof(double));
    local_w = malloc(m * sizeof(double));
    local_v = malloc(local_n * sizeof(double));

    // Adesso 0 invia a tutti un pezzo della matrice
    int num = local_n * n;

    MPI_Scatter(
        &AT[0], num, MPI_DOUBLE,
        &localAT[0], num, MPI_DOUBLE,
        0, MPI_COMM_WORLD);

    MPI_Scatter(&v[0], local_n, MPI_DOUBLE, &local_v[0], local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (me != 10)
    {

        // printf("Matrice localAT\n");
        //stampa_mat(localAT, local_n, n);
        trasp_trasp(localAT, localA, m, n, local_n);
        //  printf("Matrice localA\n");
        // stampa_mat(localA, m, local_n);

        //  printf("Vettore localv: \n");
        // for (int i = 0; i < local_n; i++)
        //     printf("%lf ", local_v[i]);
        // printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD); // sincronizzazione
    double start = MPI_Wtime();

    // // Effettuiamo i calcoli

    prod_matTra_vett(local_w, localA, m, n, local_n, local_v);
    // if (me == 0)
    // {
    //     printf("Vettore local_w: \n");
    //     for (int i = 0; i < m; i++)
    //         printf("%lf ", local_w[i]);
    //     printf("\n");
    // }

    MPI_Barrier(MPI_COMM_WORLD); // sincronizzazione
    double time = MPI_Wtime() - start;

    MPI_Reduce(&time, &T_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // 0 raccoglie i risultati parziali
    MPI_Reduce(&local_w[0], &w[0], m, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // if (me == 0)
    // {
    //     printf("w = \n");
    //     for (i = 0; i < m; i++)
    //         printf("%f ", w[i]);
    //     printf("\n");
    // }

    if (me == 0)
        printf("Tempo di esecuzione %lf s \n", T_max);

    MPI_Finalize(); /* Disattiva MPI */
    return 0;
}