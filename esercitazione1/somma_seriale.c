/*
Calcolo della somma di n numeri interi
con algoritmo seriale che prende il numero
di interi da sommare come parametro dalla linea di comando.
*/

/* Librerie da importare */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000

int main(int argc, char **argv){

    int i, somma=0, n;
    clock_t start, end;
    double t;

/*
Se non vengono passati argomenti come parametro
viene utilizzato un parametro di default.
*/
    if (argc == 1){
        n = N;
    }else{
        n = atoi(argv[1]);
    }
/* Inizializzazione variabile random */
    srand((unsigned int) time(0)); 

/* Inizio calcolo tempo */
    start = clock();

/*Calcolo interi random */
    for(i=0;i<n;i++){
        somma+=(int)rand()%5-2;
    }
/* FIne calcolo tempo */
    end = clock();
    t = (double)(end-start) / CLOCKS_PER_SEC;

/* Risultato */
    printf("Tempo di esecuzione %lf\n", t);

}