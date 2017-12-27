//Libraries

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <arpa/inet.h>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include<stdio.h>
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define no_of_connections 3
pthread_mutex_t mutex;
int sum=0,i=0,t=0,j=0;
int count=0;
bool flag=true;

void *connection_handler(void *);
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//main
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
	int portno;
	portno=atoi(argv[1]);
    //  random time for master
    srand(time(NULL));
    t=rand()%100;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

	//Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( portno );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        exit(1);
    }
    puts("bind done");

    //Listen
    listen(socket_desc , no_of_connections);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	
	while(client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = (int *)(intptr_t) malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            exit(1);
        }

        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        exit(1);
    }
    return 0;
    }

    //This will handle connection for each client

    void *connection_handler(void *socket_desc)
    {
           pthread_mutex_lock(&mutex);

    //Get the socket descriptor
		int sock = *(int*)socket_desc;
		int n;
		int ctime;
		char buffer[256];
		bzero(buffer,256);
		n = write(sock,&t,sizeof(t));
		if (n < 0) error("ERROR writing to socket");
		n = read(sock,&ctime,sizeof(ctime));
		std::cout<<"ctime"<<ctime<<"\n";
		sum=sum+ctime;
		count++;
		pthread_mutex_unlock(&mutex);
		std::cout<<ctime<<"\n";
		while(count!=no_of_connections)
		{
			sleep(2);
		}
		if(count==no_of_connections ){
			pthread_mutex_lock(&mutex);
			if(flag==true){
				flag =false;
				std::cout<<"sum"<<sum<<"\n";
				sum=sum/((no_of_connections)+1);
				std::cout<<"avg"<<sum<<"\n";
				int k;
				k=t+sum;
				std::cout<<"updated_mastertime"<<k<<"\n";
			}
			pthread_mutex_unlock(&mutex);
		}
		pthread_mutex_lock(&mutex);
		ctime=sum-ctime;
		n = write(sock,&ctime,sizeof(ctime));
		pthread_mutex_unlock(&mutex);
		return 0;
    }


