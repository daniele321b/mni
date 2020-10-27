#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
/* Scopo: definizione di una topologia a griglia bidimensionale
nproc=row*col */
int main(int argc, char **argv)
{
    int menum, nproc, menum_grid, row, col, local_n;
    int n, m, i, j;
    int dim, *ndim, reorder, *period;
    int coordinate[2], belongs[2];
    int row_rank, col_rank;
    double *v, *local_v;
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
        for (j = 0; j < n; j++)
        {
            v[j] = j;
            printf("%.2f ", v[j]);
        }
        printf("\n");
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

    MPI_Barrier(MPI_COMM_WORLD);
    printf("Final local_v [%d] = \n", menum);
    for (i = 0; i < local_n; i++)
    {
        printf("%.2f\t", local_v[i]);
    }
    printf("\n");
    /* Stampa delle coordinate */
    // printf("Processore %d coordinate nella griglia(%d, %d)\n", menum, coordinate[0],
    //        coordinate[1]);
    MPI_Finalize();
    return 0;
}
