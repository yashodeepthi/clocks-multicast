//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
	//Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	//Prepare the sockaddr_in structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
	
	//connect the server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                 error("ERROR connecting");
    int server_time;
    n=read(sockfd,&server_time,sizeof(server_time));
    printf("Server_time:%d ",server_time);
    srand(time(NULL));
    int t=rand()%100;
	int diff=0;
	diff=t-server_time;
	n = write(sockfd,&diff,sizeof(diff));
    std::cout<<"client_time"<<t<<"\n";
	int mtime,di;
    n = read(sockfd,&mtime,sizeof(mtime));
    if (n < 0)
         error("ERROR reading from socket");
	std::cout<<"difference pass to client"<<mtime<<"\n";
    t=t+mtime;
    std::cout<<"updated_client time"<<t<<"\n";
    return 0;
}
