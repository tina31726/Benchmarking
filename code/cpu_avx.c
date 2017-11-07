#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include<immintrin.h>


unsigned long  int size = 8e8;
unsigned long  int num_loop;


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
        pthread_create(&threads[i], NULL, iops, &num_loop);
    }
    for ( i = 0; i < thread_n; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&endTime, NULL);
    int Gsize = size/1e9;
   // printf("Gsize:%d size:%llu operation: %lu\n",Gsize,size,operations);
    double exec_time = 1000.0 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000.0; // total time in ms(millisecond)
    //usec- microsecond
    double flops = ((double)operations / (exec_time / 1000))/1e9;// 以Ｇiga FLOPS 為單位
    printf("With %d threads, the execution time is %10f ms and the GFlOPS is %10f\n", thread_n, exec_time, flops);


    return 0;
}

int GIOPS(int thread_n)
{
    num_loop = size/thread_n;
    struct timeval startTime, endTime;
    unsigned long operations = 4ul * size ;

    pthread_t threads[thread_n];

    gettimeofday(&startTime, NULL);    int i;
    for ( i = 0; i < thread_n; i++) {
        pthread_create(&threads[i], NULL, iops, &num_loop);
    }
    for ( i = 0; i < thread_n; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&endTime, NULL);

    double exec_time = 1000.0 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000.0;
    double iops = ((double)operations / (exec_time / 1000))/1e9;;
    printf("With %d threads, the execution time is %10f ms and the GIOPS is %10f\n", thread_n, exec_time, iops);


    return 0;
}

void * flops(void * arg)
{

    __m256 evens = _mm256_set_ps(2.1, 4.1, 6.0, 8.0, 10.4, 12.0, 14.0, 16.2);
    __m256 odds = _mm256_set_ps(1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0, 15.0);
     __m256 resulta;


     unsigned long  int *loop = (unsigned long  int*) arg;

    unsigned long  int k;

    for (k = 0; k < *loop; k++) {
          resulta = _mm256_hadd_ps(evens, odds);
          resulta = _mm256_mul_ps(evens, odds);
          resulta = _mm256_mul_ps(evens, odds);
    }

    return NULL;
}

void * iops(void * arg)
{
    __m256i evens_int = _mm256_set_epi32(2, 4, 6, 8, 10, 12, 14, 16);  
    __m256i odds_int = _mm256_set_epi32(1, 3, 5, 7, 9, 11, 13, 15);
     unsigned long  int *loop = (unsigned long  int*) arg;;
     __m256i resultd;
    unsigned long  int k;


    for (k = 0; k < *loop; k++) {
          resultd = _mm256_hadd_epi32(evens_int, odds_int);
          resultd = _mm256_mul_epi32(evens_int, odds_int);
          resultd = _mm256_mul_epi32(evens_int, odds_int);
    }

    return NULL;
}