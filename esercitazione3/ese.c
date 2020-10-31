#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
/* Scopo: definizione di una topologia a griglia bidimensionale
nproc=row*col */

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
void trasp_trasp(double *a, double *b, int m, int n, int local_n)
{

    int i, j;
    //printf("Trasposta \n \n");
    for (i = 0; i < m; i++) //righe
    {
        for (j = 0; j < local_n; j++) //colonne
        {
            b[i * local_n + j] = a[j * n + i];
            printf("B[%d]=%.2f = A[%d]=%.2f\n", i * local_n + j, b[i * local_n + j], j * n + i, a[j * n + i]);
        }
    }
}

int main(int argc, char **argv)
{
    int menum, nproc, menum_grid, row, col, local_n;
    int n, m, i, j;
    int dim, *ndim, reorder, *period;
    int coordinate[2], belongs[2];
    int row_rank, col_rank;
    double *v, *local_v, *A, *local_A, *local_TA, *row_A, *row_TA;
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
        for (j = 0; j < n; j++)
        {
            v[j] = j;
            //printf("%.2f ", v[j]);
        }
        //printf("\n");

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
    local_v = malloc(local_n * sizeof(double));

    //printf("Processore %d coordinate nella griglia(%d, %d)\n", menum, coordinate[0], coordinate[1]);

    MPI_Barrier(MPI_COMM_WORLD);
    if (coordinate[0] == 0)
    {

        MPI_Scatter(&v[0], local_n, MPI_DOUBLE, &local_v[0], local_n, MPI_DOUBLE, 0, commrow);
        // printf("local_v [%d] = \n", menum);
        // for (i = 0; i < local_n; i++)
        // {
        //     printf("%.2f\t", local_v[i]);
        // }
        // printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&local_v[0], local_n, MPI_DOUBLE, 0, commcol);
    int x = (m / row) * n;
    int y = (n / col) * m;
    row_A = malloc(x * sizeof(double));
    row_TA = malloc(x * sizeof(double));

    MPI_Barrier(MPI_COMM_WORLD);
    if (coordinate[1] == 0)
    {

        MPI_Scatter(&A[0], x, MPI_DOUBLE, &row_A[0], x, MPI_DOUBLE, 0, commcol);
        MPI_Barrier(commcol);
        //printf("m= %d n=%d localn= %d\n", m / row, n, m / row);
        //stampa_mat(row_A, m / row, n);

        //printf("Processore %d coordinate nella griglia(%d, %d)\n", menum, coordinate[0], coordinate[1]);
        //funziona matrice quadrata

        //trasp_trasp(row_A, row_TA, m, n, m / row);
        if (m == n)
        {
            trasp_trasp(row_A, row_TA, m, n, m / row);
        }
        else if (m < n)
        {
            trasp_trasp(row_A, row_TA, n, n, m);
        }
        else
        {
            trasp_trasp(row_A, row_TA, m, n, m / row);
        }
        MPI_Barrier(commcol);
        //trasp_trasp(row_A, row_TA, n, m / row, m / row);

        if (coordinate[0] == 0 && coordinate[1] == 0)
        {
            // printf("Righe me %d\n", menum);
            // stampa_mat(row_A, m / row, n);
            printf("Trasposta me %d\n", menum);
            stampa_mat(row_TA, n, m / row);
        }
    }

    int d = ((m / row) * (n / col));
    local_A = malloc(d * sizeof(double));
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(&row_TA[0], d, MPI_DOUBLE, &local_A[0], d, MPI_DOUBLE, 0, commrow);
    if (coordinate[0] == 0 && coordinate[1] == 0)
    {

        printf("Stampo local A \n");
        stampa_mat(local_A, n / col, m / row);
    }

    local_TA = malloc(d * sizeof(double));
    //trasp_trasp(local_A, local_TA, m / row, n / col, m / row);
    trasp_trasp(local_A, local_TA, m / row, m / row, n / col);

    // trasp_mat(local_A, local_TA, n / col, m / row);
    MPI_Barrier(MPI_COMM_WORLD);
    if (coordinate[0] == 0 && coordinate[1] == 0)
    {
        printf("localTA final me %d\n", menum);
        stampa_mat(local_TA, m / row, n / col);
    }

    MPI_Finalize();
    return 0;
}
