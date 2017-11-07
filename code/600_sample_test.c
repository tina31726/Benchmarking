#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



struct timeval startTime, endTime;
unsigned long long int num_sample=1;//show how many sample alreay collect
unsigned long long int  instr_table[601]={0};
double total_time;
char *label[8] ={'1','2','3','4','5','6','7','8'};
unsigned long long int   instr= 0;

void * iops(void * arg)
{
     char *message;
    message = (char *) arg;
    int sum = 0;
    int s =0;
    int a = 1;
    for ( s= 0; total_time <=600.000000; s++) //set 600 sec
    {
        sum = a+a;
        instr++;
        check_time(message,1);
       
        
    }   
    return NULL;
}


void * flops(void * arg)
{
    char *message;
    message = (char *) arg;
    float sum = 0;
    float s=0;
    float a =1.1;
    for (s = 0; total_time <=600.000000; s++) {
         sum = a+a;
        instr++;
        check_time(message,0);
    }
    
    return NULL;
}

int check_time(char *message,int type)
{
   gettimeofday(&endTime, NULL);
    double end = (double)endTime.tv_sec;
    double start = (double)startTime.tv_sec;
    total_time = end-start;
    if(total_time==1.000000 *num_sample)//先1sec test一次, initialize sample= 1;
    {
        instr_table[(int)total_time] = instr/num_sample;//
        
        num_sample = (int)total_time+1;

    }
    return 0;
}

int FLOPS_Test(int num_thread)
{
    int g=0;
    while (g<601)
    {
        instr_table[g]=0;
        g++;
    }
    instr=0;
    num_sample = 1;
    total_time = 0.0;
   

    pthread_t threads[num_thread];
    
    
    gettimeofday(&startTime, NULL);
    int i =0;

    for (i = 0; i < num_thread; i++) {
        pthread_create(&threads[i], NULL, flops, &label[i]);
    }
    for (i = 0; i < num_thread; i++) {
        pthread_join(threads[i], NULL);
    }
   gettimeofday(&endTime, NULL);
   

    printf("flops \n");
    for(i=0;i<601;i++)
    {
         printf("%d. %llu \n",i,instr_table[i]);
    }


    return 0;
}

int IOPS_Test(int num_thread)
{
    instr=0;
   
    pthread_t threads[num_thread];
    
    gettimeofday(&startTime, NULL);
    int i;
    for ( i = 0; i < num_thread; i++) {
        pthread_create(&threads[i], NULL, iops, &label[i]);
    }
    for ( i = 0; i < num_thread; i++) {
        pthread_join(threads[i], NULL);
    }
   gettimeofday(&endTime, NULL);
     
    
    printf("iops \n");
    for( i= 0;i<601;i++)
    {
         printf("%d. %llu \n",i,instr_table[i]);
    }

    
    return 0;
}


int main()
{

    int k=1;
    while(k==1)
    {
        IOPS_Test(8);
        k=0;
    }
   k=1;

    
     while(k==1)
    {
        FLOPS_Test(8);
        k=0;
    }
    
   
    return 0;
}

