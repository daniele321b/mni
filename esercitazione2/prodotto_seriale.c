#include <stdio.h>
#include <stdlib.h>

int main(){

    int n, m;

    do{
    printf("Inserisci righe \n");
    scanf("%d", &m);
    }while(m<=0);
    do{
    printf("Inserisci colonne \n");
    scanf("%d", &n);
    }while(n<=0);
}