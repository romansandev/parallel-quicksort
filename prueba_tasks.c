#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"


void llama_tarea(int p);
void tarea(int p);

int main() 
{
    const int n = 10;
    int p = 3; //variable para seleccionar el número de hilos
    omp_set_num_threads(p);
    #pragma omp parallel
        #pragma omp single
            llama_tarea(p);

return 0;
}


void llama_tarea(int p)
{
    tarea(p);
    if (p>0)
    {
        #pragma omp task
            llama_tarea(p-1);
        #pragma omp task    
            llama_tarea(p-1);
    }
}

void tarea(int p)
{
int myid = omp_get_thread_num();
int nthreads = omp_get_num_threads();
#pragma omp single
printf("soy el hilo %d de %d en la iteración %d\n", myid, nthreads, p);
}
