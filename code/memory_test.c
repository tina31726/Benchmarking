

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

volatile char *arr;
volatile char *des;
volatile char **sor;





void allocate_memory(long long int block,long long int  loop)
{
	
	sor= (char **) malloc(loop * sizeof(char *));
	long long int a=0;
 
	for(a=0; a<loop; a++)
	{
	    sor[a] = (char *) malloc(block * sizeof(char));
	    // assert(arr[i]!=NULL);
	    // memset(arr[i], 'A', block*sizeof(char));
	}
}


void creat_memory(long long int  block,long long int  loop)
{

	arr= (char *) malloc(loop * block* sizeof(char *));

	 memset(arr, 'A',loop * block* sizeof(char));


}

void creat_memory_des(long long int  block,long long int  loop)
{

	des= (char *) malloc(loop * block * sizeof(char *));
	//assert(des!=NULL);
	memset(des, 'C', loop * block*sizeof(char));

 
}

void *read_write_operations(void *n){
	

	long long int  *package = (int *) n;
  //  printf("read_write_operations %d loop, %d block\n",package[0],package[1]);

    long long int  a=0;
    for (a = 0; a < package[0]; a++) {
        memcpy(des +(package[1] *a), arr+(package[1] *a), package[1]);//copy (*m)byte from sor+mem to des+mem
    }

    return NULL;
}

/********** memory sequential write **********/

void *sequential_write(void *n){
	long long int  *loop = (long long int *)n;// m is a point variable
	long long int  a=0;

    while(a<*loop)
    {
        assert(sor[a]!=NULL);
	    memset(sor[a], 'A', sizeof(sor[a]));
	    a++;
    }

	    return NULL;
}

/********** memory random write **********/

void *random_write(void *n){

//	printf("random_read_access\n");

    long long int  *loop = (long long int *)n;// m is a point variable
    long long int  a=0;
    long long int  r;
	while(a<*loop)
    {
    	r=rand()%(*loop-1);//using rand to find a random location 
        assert(sor[r]!=NULL);
	    memset(sor[r], 'A', sizeof(sor[r]));
    	a++;
    	
    }
    return NULL;
}

/********** memory benchmark **********/

int main(){
	int j=0;
	int i=0;
	long long int  File = 1280000000;
	//long int File = 80000000;
	long long int  loop_num;
	long long int  package[2]={0};

	 

	 long long int  byte_array[4] = {8,8000,8000000,80000000};// 8 byte, 8KB, 8MB and 80MB


 	 for (i = 1; i < 9; i*=2) {// using 1 and 2 ,4, 8 thread(s)
        for (j = 0; j < 4; j++) { // block size
        	loop_num = File/byte_array[j]/i;
        	package[0] = loop_num;
        	package[1] = byte_array[j];
			int k=0;      
            struct timeval start_seq, end_seq; // timeval structure
            pthread_t pthread_seq[i]; // define a thread for sequential access


            creat_memory(byte_array[j],loop_num);
            creat_memory_des(byte_array[j],loop_num);

            gettimeofday(&start_seq, NULL);// get strating time

            for (k = 0; k < i; k++)
                pthread_create(&pthread_seq[k], NULL, read_write_operations, &package); //sequential access

            for (k = 0; k < i; k++)
                pthread_join(pthread_seq[k], NULL);// waiting thread end
            gettimeofday(&end_seq, NULL);// get ending time
               free(arr);
               free(des);
             double total_time = (1000.0 * (end_seq.tv_sec - start_seq.tv_sec) + (end_seq.tv_usec - start_seq.tv_usec) / 1000.0) ;
            double latency = total_time*1000 / File*8 ; // microsecond,us
            double throughput = File/1.0e6/(total_time/1000); // MB/sec
            
            printf("exectuion time %10f Using %d thread(s), read+write  %lli bytes, the throughput is %10f  MB/sec, the latency is %10.9f us\n",total_time, i, byte_array[j], throughput, latency);
           
                           
        }
    }
	 for (i = 1; i < 9; i*=2) {// using 1 and 2 thread(s)
        for (j = 0; j < 4; j++) { // 1 byte, 1024 bytes and 1048576 bytes
            loop_num = File/byte_array[j]/i;
          	int k=0;      
            struct timeval start_seq, end_seq; // timeval structure
            pthread_t pthread_seq[i]; // define a thread for sequential access
            allocate_memory(byte_array[j],loop_num);
            gettimeofday(&start_seq, NULL);// get strating time
           
            for (k = 0; k < i; k++)
                pthread_create(&pthread_seq[k], NULL, sequential_write, &loop_num); //sequential access
            for (k = 0; k < i; k++)
                pthread_join(pthread_seq[k], NULL);// waiting thread end
            gettimeofday(&end_seq, NULL);// get ending time
            int q=0;
            for(q=0;q<loop_num;q++)
            {
            	free(sor[q]);
            }
             double total_time = (1000.0 * (end_seq.tv_sec - start_seq.tv_sec) + (end_seq.tv_usec - start_seq.tv_usec) / 1000.0) ;
            double latency = total_time*1000 / File*8 ; // microsecond,us
            double throughput = File/1.0e6/(total_time/1000); // MB/sec
            printf("exectuion time %10f Using %d thread(s), sequential write  %lli bytes, the throughput is %10f  MB/sec, the latency is %10.9f us\n",total_time, i, byte_array[j], throughput, latency);
           
                           
        }
    }
    for (i = 1; i < 9; i*=2) {// using 1 and 2 thread(s)
        for (j = 0; j < 4; j++) {
            loop_num = File/byte_array[j]/i;
            int k=0;      
            struct timeval start_seq, end_seq; // timeval structure
            pthread_t pthread_seq[i]; // define a thread for sequential access
            allocate_memory(byte_array[j],loop_num);
            gettimeofday(&start_seq, NULL);// get strating time
            for (k = 0; k < i; k++)
                pthread_create(&pthread_seq[k], NULL, random_write, &loop_num); //sequential access
            for (k = 0; k < i; k++)
                pthread_join(pthread_seq[k], NULL);// waiting thread end
            gettimeofday(&end_seq, NULL);// get ending time
                        int q=0;
            for(q=0;q<loop_num;q++)
            {
            	free(sor[q]);
            }
            double total_time = (1000.0 * (end_seq.tv_sec - start_seq.tv_sec) + (end_seq.tv_usec - start_seq.tv_usec) / 1000.0) ;
            double latency = total_time*1000 / File*8 ; // microsecond,us
            double throughput = File/1.0e6/(total_time/1000); // MB/sec
            printf("exectuion time %10f Using %d thread(s), random write  %lli bytes, the throughput is %10f  MB/sec, the latency is %10.9f us\n",total_time,  i, byte_array[j], throughput, latency);
        }
   }
   
   
}





















