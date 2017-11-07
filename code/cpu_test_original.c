#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

unsigned long  int size = 8e8;
unsigned long int num_loop;
    float aa[1][1]={1.5};
    float bb[1][1]={0.5};
    int a[1][1]={10};
    int b[1][1]={2};



void * iops(void *);
void * flops(void *);
int GIOPS(int);
int GFLOPS(int);

int main()
{

   int i =1;
   while(i<9)
   {
    GIOPS(i);
    GFLOPS(i);
    i*=2;
   }
    
    return 0;
}

int GFLOPS(int thread_n)
{
    num_loop = size/thread_n; 
    struct timeval startTime, endTime;
    unsigned long operations = 4ul * size ;
    
    pthread_t threads[thread_n];
    
    gettimeofday(&startTime, NULL);
    int i;
    for ( i = 0; i < thread_n; i++) {
        pthread_create(&threads[i], NULL, iops,  &num_loop);
    }
    for ( i = 0; i < thread_n; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&endTime, NULL);
    
    double exec_time = 1000.0 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000.0; // total time in ms(millisecond)
    //usec- microsecond
    double flops = ((double)operations / (exec_time / 1000)) / 1e9;// 以Ｇiga FLOPS 為單位
    printf("With %d threads, the execution time is %10f ms and the GFlOPS is %10f\n", thread_n, exec_time, flops);
    
    return 0;
}

int GIOPS(int thread_n)
{
    num_loop = size/thread_n; 
    unsigned long operations = 4ul * size ;
    struct timeval startTime, endTime;
    pthread_t threads[thread_n];
    
    int i;
    gettimeofday(&startTime, NULL);
    for ( i = 0; i < thread_n; i++) {
        pthread_create(&threads[i], NULL, iops, &num_loop);
    }
    for ( i = 0; i < thread_n; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&endTime, NULL);
    
    double exec_time = 1000.0 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000.0;
    double iops = ((double)operations / (exec_time / 1000)) / 1e9;
    printf("With %d threads, the execution time is %10f ms and the GIOPS is %10f\n", thread_n, exec_time, iops);
    
    return 0;
}

void * flops(void * arg)
{
    unsigned long  int *loop = (unsigned long  int*) arg;
    unsigned long  int k;

    float c;
    
    
    for (k = 0; k < *loop ; k++) {
        c = aa[0][0] + bb[0][0];
        c = aa[0][0] - bb[0][0];
        c = aa[0][0] * bb[0][0];
        c = aa[0][0] / bb[0][0];

    }
    
    return NULL;
}

void * iops(void * arg)
{
    unsigned long  int *loop = (unsigned long  int*) arg;
    unsigned long  int k;

    int c;
    
    
    for (k = 0; k < *loop ; k++) {
        c = a[0][0] + b[0][0];
        c = a[0][0] - b[0][0];
        c = a[0][0] * b[0][0];
        c = a[0][0] / b[0][0];
    }
    
    return NULL;
}