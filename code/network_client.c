//from client side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>

//port number
#define PORT 8888
//buffer size to be sent
//packet size is 64kb, so we set buffer size 64000
#define BUFFER_SIZE 64000
//define below NUM_LOOPS will guarantee the amount of transfer data is 8GB, 8GB/64KB/2=62500
#define NUM_LOOPS 100

//define server's socket address
struct sockaddr_in ser_addr;

//TCP
void *Cli_TCP(void *arg)
{
//define the file descriptor of socket system call	
        int sockfd;
        int  *num_thread = (int*)arg;
        char *ack= (char*)malloc(BUFFER_SIZE);


        char *str = (char*)malloc(BUFFER_SIZE);
//        char server_reply[15];
//Initialize the str with 0s
        bzero(str, BUFFER_SIZE);
	
//create a socket, choose TCP for stream sockets
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Socket creating failed.\n");
	}


//connect to the server
	if(connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1)
	{
		printf("Connection failed.\n");
	}

//	printf("Start to send message to server. \n");

//send messages to the server
int k;
for(k = 0; k < NUM_LOOPS/(*num_thread); k++)
	{
		send(sockfd, str, BUFFER_SIZE, 0);
//                recv(sockfd, ack, BUFFER_SIZE, 0);
	}

close(sockfd);
}


//client UDP
void *Cli_UDP(void *arg)
{	
//define the file descriptor of socket system call	
        int sockfd;
        int  *num_thread = (int*)arg;
int addr_size=sizeof(struct sockaddr_in);

        char *str = (char*)malloc(BUFFER_SIZE);
//Initialize the str with 0s
        bzero(str, BUFFER_SIZE);
	
//create a socket, choose UDP for DGRAM sockets
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		printf("Socket creating failed.\n");
	}

	
//start send messages to server
int k;
	for(k = 0; k < NUM_LOOPS/(*num_thread); k++)
	{
         
                 sendto(sockfd, str, BUFFER_SIZE, 0, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
recvfrom(sockfd, str, BUFFER_SIZE, 0,(struct sockaddr *)&ser_addr, &addr_size);
	}
	
	//close socket
	close(sockfd);
}


int main(int argc, char const *argv[])
{

  if(argc != 3)//
	{
                printf("Please Type protocol: tcp, udp\ncocurrency: 1, 2, 4, 8\n");
		return -1;
	}
   
// argv[1] tcp / udp
// argv[2] 1 thread / 2 threads / 4 threads / 8 threads
// argv[3] server ip address

        int thread_id = atoi(argv[2]);
	pthread_t pthreads[thread_id];

//define the host computer on the internet
        struct hostent *host;
//        host = (struct hostent*) gethostbyname(argv[3]);
	
//set server's address and port number
	ser_addr.sin_family = AF_INET;
//	ser_addr.sin_addr = *((struct in_addr*)host->h_addr);
ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(PORT);

struct timespec sta_time, end_time;

    clock_gettime(CLOCK_REALTIME, &sta_time);    
	
	int k;
        if(strcmp(argv[1], "tcp") == 0)
	{


		for (k = 0; k < thread_id; k++)
		{
			pthread_create(&pthreads[k], NULL, Cli_TCP, &thread_id);
		}
                for (k = 0; k < thread_id; k++)
	        {
		        pthread_join(pthreads[k], NULL);
	        }


	}

	else if(strcmp(argv[1], "udp") == 0)
	{

		for (k = 0; k < thread_id; k++)
		{
			pthread_create(&pthreads[k], NULL, Cli_UDP, &thread_id);
		}
                for (k = 0; k < thread_id; k++)
	        {
		        pthread_join(pthreads[k], NULL);
	        }

	}
clock_gettime(CLOCK_REALTIME, &end_time);
	
double startTime = 1000 * sta_time.tv_sec + sta_time.tv_nsec / 1.0e6;
        double endTime = 1000 * end_time.tv_sec + end_time.tv_nsec / 1.0e6;

        float throughput = (float) BUFFER_SIZE* NUM_LOOPS *2 / 1000000 / ((endTime - startTime) /1000 );


	double latency = (endTime - startTime) / NUM_LOOPS;

	
	printf("The throughput is %10lf Mbps, latency is%10lf ms\n. ", throughput, latency);
	
	return 0;
}
	
