//from server side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

//port number
#define PORT 8888
//set fixed buffer size 64 kb
#define BUFFER_SIZE 64000
//define below NUM_LOOPS will guarantee the amount of transfer data is 8GB, 8GB/64KB/2=62500
#define NUM_LOOPS 100


struct arg
{
     int sock;
     int num_thread;
};

//Read Data under TCP protocol
void *tcpRead(void *arg)
{

char *str = (char*)malloc(BUFFER_SIZE);
//	int rec = BUFFER_SIZE;
        
struct arg *argtcp = (struct arg*)arg; 
//int  acc_sockfd = argtcp;
//Initialize the str with 0s

        bzero(str, BUFFER_SIZE);
//once client connecting to server, server read the message from client
int rec = NUM_LOOPS;
while(rec>0)
//for(k = 0; k < NUM_LOOPS/(argtcp->num_thread); k++)
{
//transfer ping pong message
           rec=recv(argtcp->sock, str, BUFFER_SIZE,0);

            send(argtcp->sock, str, BUFFER_SIZE, 0);

}



}


//TCP
void *Ser_TCP(int num_thread)
{
//define the file descriptor of socket system call and accept system call as sockfd, acc_sockfd; rec is the value from received calls
	int sockfd, acc_sockfd, rec;
//defile socket address for server and client
	struct sockaddr_in ser_addr, cli_addr;
	
//create a socket, choose TCP for stream sockets
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Socket creating failed.\n");
	}

//        printf("tid%d\n",*tid);
	//set server address and port
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        ser_addr.sin_port = htons(PORT);

//allow reuse of port
 //rec = 1;
 //if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&rec,sizeof(int)) == -1) {
 // perror("setsockopt");
 // exit(1);
// }
	//bind port to the socket
	//if(bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr))== -1)
if(bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr))== -1)

	{
		printf("Binding failed.\n");
	}
	
//start listening the port, the number of connection defined by 5, which is the max size permitted by most system
	listen(sockfd, 5);

	
	//connected with the client
        int cli_size = sizeof(cli_addr);
//	acc_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_size);
	if(acc_sockfd == -1)
	{
		printf("Acceptance failed.\n");
	}

	
        pthread_t pthreads[num_thread];
 //       struct arg * argtcp = (struct arg*)malloc(sizeof(struct arg));
   //     argtcp->sock =  acc_sockfd;
     //   argtcp->num_thread = num_thread;
//int k=0;
  //      for (k = 0; k < num_thread; k++)
	//	{
//
//			pthread_create(&pthreads[k], NULL, tcpRead, argtcp);
//		}
  //              for (k = 0; k < num_thread; k++)
	//        {
	//	        pthread_join(pthreads[k], NULL);
	  //      }
int i = 0;
	while (1) {
acc_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_size);
//after the new client is connected to the server, issue a thread to handle the request from this specific client
		struct arg * argtcp = (struct arg*)malloc(sizeof(struct arg));
		argtcp->sock =  acc_sockfd;
        argtcp->num_thread = num_thread;
		pthread_create(&pthreads[i], NULL, tcpRead, argtcp);
		i++;
	}
        
         
//close socket
	close(acc_sockfd);
	close(sockfd);
}

//Read data under UDP protocol
void *udpRead(void *arg)
{

char *str = (char*)malloc(BUFFER_SIZE);
	int rec = BUFFER_SIZE;
        
        struct sockaddr_in cli_addr;
        struct arg *argudp = (struct arg*)arg; 
//int  sockfd = (int*)sock;

        int cli_size = sizeof(cli_addr);


//Initialize the str with 0s
        bzero(str, BUFFER_SIZE);
		
//receive messages from client
int k;

for(k = 0; k < NUM_LOOPS/(argudp->num_thread); k++){
recvfrom(argudp->sock, str, BUFFER_SIZE, 0,(struct sockaddr *)&cli_addr, &cli_size);
sendto(argudp->sock, str, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));

}

}


//UDP
void *Ser_UDP(int num_thread)
{
//define the file descriptor of socket system call and accept system call as sockfd, acc_sockfd; rec is the value from received calls
	int sockfd, rec;
	struct sockaddr_in ser_addr;
//	socklen_t addrlen;

//	int tid = (int)(long)thread_id;
//create a socket, choose UDP for DGRAM sockets
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		printf("Socket creating failed.\n");
	}

//set server address and port
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(PORT);
	
//bind port to the socket
	if(bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr))== -1)
	{
		printf("Binding failed.\n");
	}

	pthread_t pthreads[num_thread];
        struct arg * argudp = (struct arg*)malloc(sizeof(struct arg));
        argudp->sock =  sockfd;
        argudp->num_thread = num_thread;

int k;
        for (k = 0; k < num_thread; k++)
		{
			pthread_create(&pthreads[k], NULL, udpRead, argudp);
		}
                for ( k = 0; k < num_thread; k++)
	        {
		        pthread_join(pthreads[k], NULL);
	        }

//close socket
	close(sockfd);
}

int main(int argc, char const *argv[])
{
        if(argc != 3)
	{
                printf("Please Type protocol: tcp, udp\ncocurrency: 1, 2, 4, 8\n");
		return -1;
	}
   
// argv[1] tcp / udp
// argv[2] 1 thread / 2 threads / 4 threads

        int thread_id = atoi(argv[2]);
	pthread_t pthreads[thread_id];
        
	
	if(strcmp(argv[1], "tcp") == 0)
	{
		Ser_TCP(thread_id);
	}
	else if(strcmp(argv[1], "udp") == 0)
	{
		Ser_UDP(thread_id);
	}
	

	return 0;
}
