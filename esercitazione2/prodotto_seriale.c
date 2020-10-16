#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

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

int main(int argc, char **argv)
{

    int n, m, j, i;
    double *A, *v, *w;
    double time;

    if (argc < 3)
    {
        do
        {
            printf("Inserisci righe \n");
            scanf("%d", &m);
        } while (m <= 0);
        do
        {
            printf("Inserisci colonne \n");
            scanf("%d", &n);
        } while (n <= 0);
    }
    else
    {
        m = atoi(argv[1]);
        n = atoi(argv[2]);
    }

    A = malloc(m * n * sizeof(double));
    v = malloc(sizeof(double) * n);
    w = malloc(sizeof(double) * m);

    // printf("vettore v: \n");
    for (j = 0; j < n; j++)
    {
        v[j] = j;
        // printf("%f ", v[j]);
    }
    // printf("\n");

    //printf("matrice A: \n");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (j == 0)
                A[i * n + j] = 1.0 / (i + 1) - 1;
            else
                A[i * n + j] = 1.0 / (i + 1) - pow(1.0 / 2.0, j);
            //printf("%f ", A[i * n + j]);
        }
        // printf("\n");
    }
    double start = MPI_Wtime();
    prod_mat_vett(w, A, m, n, v);
    time = MPI_Wtime() - start;
    //printf("Vettore risultato w = \n");
    // for (i = 0; i < m; i++)
    //  printf("%f ", w[i]);
    // printf("\n");

    printf("Tempo di esecuzione %lf s \n", time);
}