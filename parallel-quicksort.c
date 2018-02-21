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
    
    if(threads <= 1 || end - start < 2) //si solo se ha asignado un hilo a esta parte (sea L o U) o si sólo hay un número en ella, salgo de la recursión parallela
        return;                 //habría que añadir que, en caso de que haya más de un número, debería realizarse para el único hilo con el que se ha llamado a esta recursión el algoritmo no paralelo

    int pivot, i, u = 0; //u es una variable que guardará el número de elementos menores que el pivote
    
    int* lvec = calloc(end - start, sizeof(int)); //un vector que tendrá 1s en las posiciones en que el array tenga elementos menores o iguales que el pivote y 0s en las que los elementos sean mayores
                                                //esta asignación de 1s y 0s se hace una vez los trozos del array se han ordenado en paralelo según el pivote
    
    int* copia = malloc(sizeof(int) * (end - start)); //servirá como vector auxiliar para no pisar los datos en el array original

    srand(time(NULL));
    pivot = array[rand() % (end - start) + start];
    
    printf("pivot: %d\n", pivot);

    for(i = 0; i < threads; i++) //se lanzan tantas tareas como hilos se hayan asignado a el array (o trozo del array) repartiendo las posiciones equitativamente entre los hilos
    {
        if (i+1 == threads)
        {   
            #pragma omp task    
                ordena(array, start + (i*((end - start) / threads)), end, pivot, lvec); 
        }
        else
        {
            
            #pragma omp task    
                ordena(array, start + (i*((end - start) / threads)), start + ((i + 1)*((end - start) / threads)), pivot, lvec);
        }
    }

    #pragma omp taskwait //esperamos a que los trozos hayan sido ordenados
    
    for (i = start; i < end; i++) //rellenamos la variable @u con el número total de elementos menores que el pivote
        if(lvec[i] == 1)
            u++;

    for(i = 0; i < threads; i++) //uno las partes menores que el pivote a la izquierda del vector y las mayores a la derecha en paralelo con tareas 
        if (i+1 == threads)
            #pragma omp task
                merge(array,  start + (i*((end - start) / threads)), end, lvec, u, copia);
        else
            #pragma omp task
                merge(array,  start + (i*((end - start) / threads)),  start + ((i + 1)*((end - start) / threads)), lvec, u, copia);

    #pragma omp taskwait

    //en este punto tengo en @copia el vector ordenado para esta iteración

    //En función al tamaño de la parte menor y mayor que el pivote, reparto el número de hilos disponible
    int repartohilos = (int) (((float) u / (end - start) ) * threads + 0.5 );
    int rpeartohilos2 = (int) (((float) ((end - start) - u) / (end - start) ) * threads + 0.5 );


    //copio @copia en @array, esto debería hacerse sólo al final, pasando simplemente @copia durante la recursión
    for(i = start; i < end; i++)
        array[i] = copia[i];
    /*
    #pragma omp task
        quicksort(array, start, start + u, repartohilos);
    #pragma omp task
        quicksort(array, start + u, end, rpeartohilos2);*/

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
