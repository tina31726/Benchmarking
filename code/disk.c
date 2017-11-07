//  disk_benchmark

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

//below DATA_SIZE  which is data size equals to 10000000000 will guarantee operations will be performed within a file with size 10GB
//This file has been created before doing below experiment, we just need to execute writeFile.c, and then a binary file named new_file.bin
//which have size 10GB will be created.
#define DATA_SIZE 10000000000
//below NUM_LOOPS is the operation data size once the 10GB file created
#define NUM_LOOPS 1000000000

int Read_Write(int, int);
int Read_Ran(int, int);
int Read_Seq(int, int);
void * call_Read_Write(void *);
void * call_sequential_Read(void *);
void * call_random_Read(void *);

struct arg
{
     int block_size;
     int num_thread;
};

int Read_Write(int block_size, int thread_id)
{
    pthread_t pthreads[thread_id];
    struct timespec sta_time, end_time;
struct arg *arg = (struct arg*)malloc(sizeof(struct arg)); 
        arg->block_size =  block_size;
        arg->num_thread = thread_id;
    
    clock_gettime(CLOCK_REALTIME, &sta_time);
	
	int k;
	
    for (k = 0; k < thread_id; k++) {
        pthread_create(&pthreads[k], NULL, call_Read_Write, arg);
    }
    for (k = 0; k < thread_id; k++) {
        pthread_join(pthreads[k], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    double startTime = 1000 * sta_time.tv_sec + sta_time.tv_nsec / 1.0e6;
    double endTime = 1000 * end_time.tv_sec + end_time.tv_nsec / 1.0e6;

    
    double read_writeTime = endTime - startTime; // total time in ms
    double throughput = (NUM_LOOPS+DATA_SIZE*0.05) / 1.0e6/ (read_writeTime / 1000.0); // MBps
    double latency = read_writeTime / (NUM_LOOPS+DATA_SIZE*0.05)* 8 ; 
//use 8bk size to measure latency, ms per 8b block_size
    
    printf("With %d threads, for read+write from disk, the throughput is %10lf MBps and the latency is %10lf ms\n, the run time is %10lf s\n", thread_id, throughput, latency,read_writeTime/1000);
    
    return 0;
}

void * sequential_Write(int b_size)
{
    
//create a new file
    
    FILE *new_file;
    
    
//open a binary file for writhing in binary mode, if file exists, rewrite it; else create new file.
    new_file = fopen("new_file.bin", "wb");

//disable the buffer so that following process are executed directly to disk
    setbuf(new_file, NULL);

    if(new_file == NULL){
          printf("Cannot open a binary file!!");
          exit(1);
   
}

    int block_size = (int)b_size;

//create a string with size of block_size and fill out memory
    char * str = (char *)malloc(block_size);
    memset(str, 0, sizeof(str));

//re-write the string to the new_file.bin in multiple times; and the number of times of writing inversely propotional o the block size.
//Using DATA_SIZE/block_size as the re-write number of loops, it will ensure the re-write is within new_file.bin.

int i;
    for (i = 0; i < DATA_SIZE / block_size; i++) {
       fwrite(str, 1, block_size, new_file);
    }

    fclose(new_file);	//close the new_file
    free(str);     //free cache
    
    return (0);
}


int Read_Seq(int block_size, int thread_id)
{
    pthread_t pthreads[thread_id];
    struct timespec sta_time, end_time;
struct arg *arg = (struct arg*)malloc(sizeof(struct arg));
       arg->block_size =  block_size;
        arg->num_thread = thread_id;
    
    clock_gettime(CLOCK_REALTIME, &sta_time);
	
	int k;
	
    for (k = 0; k < thread_id; k++) {
        pthread_create(&pthreads[k], NULL, call_sequential_Read, arg);
    }
    for (k = 0; k < thread_id; k++) {
        pthread_join(pthreads[k], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    double startTime = 1000 * sta_time.tv_sec + sta_time.tv_nsec / 1.0e6;
    double endTime = 1000 * end_time.tv_sec + end_time.tv_nsec / 1.0e6;
	
    
    double readTime = endTime - startTime; // total time in ms
    double throughput = NUM_LOOPS / 1.0e6 / (readTime / 1000.0); // MBps
    double latency = readTime / NUM_LOOPS * 8 ; //use 8bk size to measure latency, ms per 8b block_size
    
    printf("With %d threads, for sequencial reading from disk, the throughput is %10lf MBps and the latency is %10lf ms\n, the run time is %10lf s\n", thread_id, throughput, latency, readTime/1000);
    
    return 0;
}

int Read_Ran(int block_size, int thread_id)
{
    pthread_t pthreads[thread_id];
    struct timespec sta_time, end_time;
struct arg *arg = (struct arg*)malloc(sizeof(struct arg));; 
        arg->block_size =  block_size;
        arg->num_thread = thread_id;
    
    clock_gettime(CLOCK_REALTIME, &sta_time);
	
	int k;
    for (k = 0; k < thread_id; k++) {
        pthread_create(&pthreads[k], NULL, call_random_Read, arg);
    }
    for (k = 0; k < thread_id; k++) {
        pthread_join(pthreads[k], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    double startTime = 1000 * sta_time.tv_sec + sta_time.tv_nsec / 1.0e6;
    double endTime = 1000 * end_time.tv_sec + end_time.tv_nsec / 1.0e6;
	
    
    double readTime = endTime - startTime; 
    double throughput = NUM_LOOPS /1.0e6 / (readTime / 1000.0); 
    double latency = readTime / NUM_LOOPS * 8 ; //use 8bk size to measure latency, ms per 8b block_size
    
    printf("With %d threads, for random reading from disk, the throughput is %10lf MBps and the latency is %10lf ms\n, the run time is %10lf s\n", thread_id, throughput, latency, readTime/1000);
    
    return 0;
}


void * call_Read_Write(void *arg)
{
    
//create a new file
    FILE *new_file;
    
//open a binary file for writhing in binary mode, if file exists, rewrite it; else create new file.
    new_file = fopen("new_file.bin", "rb+");

//disable the buffer so that following process are executed directly to disk
    setbuf(new_file, NULL);

    if(new_file == NULL){
          printf("Cannot open a binary file!!");
          exit(1);
   
}

    struct arg *argnew = (struct arg*)arg; 

//create a string with size of block_size and fill out the memory
    char * str = (char *)malloc(argnew->block_size);
char * str1 = (char *)malloc(argnew->block_size);
    memset(str, 0, sizeof(str));

//re-read and re-write to the new_file
    fseek(new_file, 0, SEEK_SET);
int i;
    for (i = 0; i < NUM_LOOPS / (argnew->block_size)/(argnew->num_thread); i++) {
      
     fread(str1, (argnew->block_size), 1, new_file);         
}

fseek(new_file,0 , SEEK_SET);
  for (i = 0; i < DATA_SIZE *0.05/ (argnew->block_size)/(argnew->num_thread); i++) {  

fwrite(str, (argnew->block_size), 1, new_file);      

        
}

    fclose(new_file);	//close the new_file
    free(str);
    return (0);
}


void * call_sequential_Read(void * arg)
{
//create a new file
 //   FILE *new_file;
 
    
//open the input file that has been writen.
   FILE *new_file = fopen("new_file.bin", "rb");

//disable the buffer so that following process are executed directly to disk
    setbuf(new_file, NULL);

   if(new_file == NULL){
          printf("Cannot open the input file!!");
          exit(1);
   
}
    struct arg *argnew = (struct arg*)arg; 
    char * str = (char *)malloc(argnew->block_size);

//Read the file, seek to the beginning of the file
fseek(new_file, 0, SEEK_SET);
    
	int i;
    for (i = 0; i < NUM_LOOPS / (argnew->block_size)/(argnew->num_thread); i++) {
     
       fread(str, argnew->block_size, 1, new_file);
    }
    
    fclose(new_file);
    free(str);
    
    return (0);
}

void * call_random_Read(void * arg)
{
 

//open the input file that has been writen.
   FILE *new_file = fopen("new_file.bin", "rb");

//disable the buffer so that following process are executed directly to disk
    setbuf(new_file, NULL);

   if(new_file == NULL){
          printf("Cannot open the input file!!");
          exit(1);
   
}
 
    srand(time(NULL)); 
    
 //   FILE *new_file = fopen("test.bin", "wb");
struct arg *argnew = (struct arg*)arg; 
   
    
    char * str = (char *)malloc(argnew->block_size);

	int i;
    for (i = 0; i < NUM_LOOPS / (argnew->block_size)/(argnew->num_thread); i++) {
//keep reseting pointer at random position after each reading loop

       int ran_pointer = (rand()%(NUM_LOOPS/argnew->block_size))*(argnew->block_size);
        fseek(new_file , ran_pointer, SEEK_SET);
         fread(str, (argnew->block_size), 1, new_file);
    }
    
    fclose(new_file);
    free(str);
    
    
}

int main(int argc, char const *argv[])
{
   if(argc != 4)
	{
                printf("Please Type operation: Read_Write, Read_Seq, Read_Ran\nblock size: 8B, 8KB, 8MB, 80MB\ncocurrency: 1, 2, 4, 8\n");
		return -1;
	}
    
    // argv[0] disk_benchmark
    // argv[1] Read_Write / Read_Seq / Read_Ran
    // argv[2] 8B/ 8KB/ 8MB/ 80MB
    // argv[3] 1 thread / 2 threads / 4 / 8 threads
    int thread_n, block_size;
    thread_n = atoi(argv[3]);
    if (strcmp(argv[2], "8B") == 0) {
        block_size = 8;
    }
    else if (strcmp(argv[2], "8KB") == 0){
        block_size = 8 * 1000;
    }
    else if (strcmp(argv[2], "8MB") == 0){
        block_size = 8 * 1000 * 1000;
    }
    else if (strcmp(argv[2], "80MB") == 0){
        block_size = 8 * 1000 * 1000 * 10;
    }
    else{
        printf("The block size is not correct.\n");
        return -1;
    }
    
        if (strcmp(argv[1], "Read_Write") == 0) {
//if read+write mode, then read the file and re-write the file within new_file.bin, only counting the read and rewrite run time
            Read_Write(block_size, thread_n);
        }
//if read mode, then open and write a file first, then read the file within new_file.bin, only counting the read run time
        else if (strcmp(argv[1], "Read_Seq") == 0){
           Read_Seq(block_size, thread_n);
        }
        else if (strcmp(argv[1], "Read_Ran") == 0){
            Read_Ran(block_size, thread_n);
        }
        else{
            printf("Invalid access type.\n");
            return -1;
        }
    return 0;
}



