#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"

void quicksort(int *array, int start, int end, int threads);
int ordena(int *array, int start, int end, int pivot, int* lvec);
void merge(int* array, int start, int end, int* lvec, int u, int* copia);
int main() 
{
    const int n = 9;
    const int p = 3;
    omp_set_num_threads(p);
    int a[] = { 7, 12, 1, -2, 0, 15, 4, 11, 9};

    int i;

    printf("\n\nVector desordenado:  ");
    for(i = 0; i < n; ++i)
      printf(" %d ", a[i]);
    printf("\n");
    #pragma omp parallel
    #pragma omp single    
    quicksort( a, 0, n, p);

    printf("\n\nVector ordenado:  ");
    for(i = 0; i < n; ++i)
      printf(" %d ", a[i]);
    printf("\n");

    return 0;
}
 
void quicksort(int *array, int start, int end, int threads)
{
    int pivot, i, numL = 0, u = 0;
    int* lvec = calloc(end - start, sizeof(int));
    int* copia = malloc(sizeof(int) * (end - start));
    pivot = array[start];

    if(threads <= 1)
        return;
    for(i = start; i <= end - (end - start)/threads; i += (end - start)/threads)
        #pragma omp task
            ordena(array, i, i + (end - start)/threads, pivot, lvec);

    #pragma omp taskwait
    
    for (i = start; i < end; i++)
        if(lvec[i] == 1)
            u++;
    for(i = start; i <= end - (end - start)/threads; i += (end - start)/threads)
        #pragma omp task
            merge(array, i, i + (end - start)/threads, lvec, u, copia);

    #pragma omp taskwait

    for(i = start; i < end; i++)
        array[i] = copia[i];


    int repartohilos = (int) (((float) u / (end - start) ) * threads + 0.5 );

    //printf("quicksort(array, %d, %d, %d);\n", start, start + u, repartohilos);

    //printf("quicksort(array, %d, %d, %d);\n", start + u, end, threads - repartohilos);
/*
    #pragma omp task
        quicksort(array, start, start + u, repartohilos);

    #pragma omp task
        quicksort(array, start + u, end, threads - repartohilos);*/

}


void merge(int* array, int start, int end, int* lvec, int u, int* copia)
{
int l = 0, i;

for (i = 0; i < start; i++)
    if(lvec[i]==1)
        l++;
    else
        u++;

for (i = start; i < end; i++)
    if(lvec[i] == 1)
    {
        copia[l] = array[i];
        l++;
    }
    else
    {
        copia[u] = array[i];
        u++;       
    }

}


int ordena(int *array, int start, int end, int pivot, int* lvec)
{
    int i, tmp, numElementos = 0;

    for(i = start; i < end; i++)
        if (array[i] <= pivot)
        {
            lvec[start] = 1;
            numElementos++;
            tmp = array[start];
            array[start] = array[i];
            array[i] = tmp;
            start++;
        }
    return numElementos; 
}
