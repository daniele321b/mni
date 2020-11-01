#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
/* Scopo: definizione di una topologia a griglia bidimensionale
nproc=row*col */

void trasposta(double **A, double **B, int m, int n)
{
    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            B[j][i] = A[i][j];
        }
    }
}

void convertiAB(double *A, double **B, int m, int n)
{
    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            B[i][j] = A[(i * n) + j];

            //printf("B[%d][%d]%.2f =A[%d]=%.2f\n", i, j, B[i][j], (i * n) + j, A[(i * n) + j]);
        }
    }
}

void convertiBA(double **A, double *B, int m, int n)
{
    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            B[(i * n) + j] = A[i][j];
            //printf("B[%d]=%.2f = A[%d][%d]=%.2f\n", (i * n) + j, B[(i * n) + j], i, j, A[i][j]);
        }
    }
}

void stampa_mat(double *a, int m, int n)
{
    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
            printf("%.2f\t", a[i * n + j]);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    int menum, nproc, menum_grid, row, col, local_n, local_m;
    int n, m, i, j;
    int dim, *ndim, reorder, *period;
    int coordinate[2], belongs[2];
    int row_rank, col_rank;
    double *v, *local_v, *A, *local_A, *local_TA, *row_A, *row_TA;
    double **row_B, **row_TB;
    double **local_B, **local_TB;

    MPI_Comm comm_grid, commrow, commcol; /* definizione di tipo communicator */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &menum);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    /* Numero di righe della griglia di processo */
    if (menum == 0)
    {
        printf("Numero di righe della griglia\n");
        scanf("%d", &row);

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

        v = malloc(sizeof(double) * n);
        A = malloc(m * n * sizeof(double));

        //Inizializzo v
        printf("Vettore: \n");
        for (j = 0; j < n; j++)
        {
            v[j] = j;
            printf("%.2f ", v[j]);
        }
        printf("\n");
        printf("<----------------------------------->\n");
        printf("Matrice: \n");
        //inizializzo A
        for (i = 0; i < m; i++)
        {
            for (j = 0; j < n; j++)
            {
                if (j == 0)
                    A[i * n + j] = 1.0 / (i + 1) - 1;
                else
                    A[i * n + j] = 1.0 / (i + 1) - pow(1.0 / 2.0, j);
                printf("%.2f ", A[i * n + j]);
            }
            printf("\n");
        }
        printf("<----------------------------------->\n");
    }
    /* Spedizione di row da parte di 0 a tutti i processori */
    MPI_Bcast(&row, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    dim = 2;           /* Numero di dimensioni della griglia */
    col = nproc / row; /* Numero di colonne della griglia */
    /* vettore contenente le lunghezze di ciascuna dimensione*/
    ndim = (int *)calloc(dim, sizeof(int));
    ndim[0] = row;
    ndim[1] = col;
    /* vettore contenente la periodicità delle dimensioni */
    period = (int *)calloc(dim, sizeof(int));
    period[0] = period[1] = 0;
    reorder = 0;
    /* Definizione della griglia bidimensionale */
    MPI_Cart_create(MPI_COMM_WORLD, dim, ndim, period, reorder, &comm_grid);
    MPI_Comm_rank(comm_grid, &menum_grid); /* id nella griglia */
    /* Definizione delle coordinate di ciascun processo nella griglia bidimensionale */
    MPI_Cart_coords(comm_grid, menum_grid, dim, coordinate);

    /* crea le sottogriglie di righe*/
    belongs[0] = 0;
    belongs[1] = 1; /*dim. variabile*/
    MPI_Cart_sub(comm_grid, belongs, &commrow);
    /* crea le sottogriglie di colonne*/
    belongs[0] = 1; /* dim. variabile*/
    belongs[1] = 0;
    MPI_Cart_sub(comm_grid, belongs, &commcol);
    local_n = n / col;
    local_m = m / row;
    local_v = malloc(local_n * sizeof(double));

    //printf("Processore %d coordinate nella griglia(%d, %d)\n", menum, coordinate[0], coordinate[1]);

    MPI_Barrier(MPI_COMM_WORLD);
    if (coordinate[0] == 0)
    {
        MPI_Scatter(&v[0], local_n, MPI_DOUBLE, &local_v[0], local_n, MPI_DOUBLE, 0, commrow);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (coordinate[0] == 0 && coordinate[1] == 0)
    {
        printf("Vettore locale %d\n", menum);
        for (j = 0; j < local_n; j++)
        {
            printf("%.2f ", local_v[j]);
        }
        printf("\n");
    }
    MPI_Bcast(&local_v[0], local_n, MPI_DOUBLE, 0, commcol);
    int x = local_m * n;
    row_A = malloc(x * sizeof(double));
    row_TA = malloc(x * sizeof(double));
    MPI_Barrier(MPI_COMM_WORLD);
    if (coordinate[1] == 0)
    {

        MPI_Scatter(&A[0], x, MPI_DOUBLE, &row_A[0], x, MPI_DOUBLE, 0, commcol);
        MPI_Barrier(commcol);

        row_B = (double **)malloc(local_m * sizeof(double *));
        row_B[0] = (double *)malloc(local_m * n * sizeof(double));
        for (i = 1; i < local_m; i++)
        {
            row_B[i] = row_B[0] + (i * n);
        }

        row_TB = (double **)malloc(n * sizeof(double *));
        row_TB[0] = (double *)malloc(local_m * n * sizeof(double));
        for (i = 1; i < n; i++)
        {
            row_TB[i] = row_TB[0] + (i * local_m);
        }

        convertiAB(row_A, row_B, local_m, n);

        trasposta(row_B, row_TB, local_m, n);

        convertiBA(row_TB, row_TA, n, local_m);

        MPI_Barrier(commcol);

        if (coordinate[0] == 0 && coordinate[1] == 0)
        {

            printf("ROW_A %d\n", menum);
            stampa_mat(row_A, local_m, n);

            // for (int i = 0; i < local_m; i++)
            // {
            //     for (int j = 0; j < n; j++)
            //     {
            //         printf("%.2f\t", row_B[i][j]);
            //     }
            //     printf("\n");
            // }

            // for (int i = 0; i < n; i++)
            // {
            //     for (int j = 0; j < local_m; j++)
            //     {
            //         printf("%.2f\t", row_TB[i][j]);
            //     }
            //     printf("\n");
            // }

            // printf("ROW_A Trasposta me %d\n", menum);
            // stampa_mat(row_TA, n, local_m);
            free(row_B);
            free(row_TB);
        }
    }

    int d = ((m / row) * (n / col));
    local_A = malloc(d * sizeof(double));
    local_TA = malloc(d * sizeof(double));

    local_B = (double **)malloc(local_n * sizeof(double *));
    local_B[0] = (double *)malloc(local_m * local_n * sizeof(double));
    for (i = 1; i < local_n; i++)
    {
        local_B[i] = local_B[0] + (i * local_m);
    }

    local_TB = (double **)malloc(local_m * sizeof(double *));
    local_TB[0] = (double *)malloc(local_m * local_n * sizeof(double));
    for (i = 1; i < local_m; i++)
    {
        local_TB[i] = local_TB[0] + (i * local_n);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(&row_TA[0], d, MPI_DOUBLE, &local_A[0], d, MPI_DOUBLE, 0, commrow);
    convertiAB(local_A, local_B, local_n, local_m);

    trasposta(local_B, local_TB, local_n, local_m);

    convertiBA(local_TB, local_TA, local_m, local_n);
    if (coordinate[0] == 0 && coordinate[1] == 0)
    {
        // printf("local A \n");
        // stampa_mat(local_A, local_m, local_n);

        // printf("Local B \n");
        // for (int i = 0; i < local_m; i++)
        // {
        //     for (int j = 0; j < local_n; j++)
        //     {
        //         printf("%.2f\t", local_B[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("Local TB \n");
        // for (int i = 0; i < local_n; i++)
        // {
        //     for (int j = 0; j < local_m; j++)
        //     {
        //         printf("%.2f\t", local_TB[i][j]);
        //     }
        //     printf("\n");
        // }

        printf("Stampo local A \n");
        stampa_mat(local_TA, local_m, local_n);
    }

    MPI_Finalize();
    return 0;
}
