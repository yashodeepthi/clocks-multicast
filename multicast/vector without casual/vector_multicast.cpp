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
#include <pthread.h>
#include <time.h>
#include <queue>

void *multicast(void *);
void *listerner(void *);
struct hostent *server;
int PORT[3],sum=0,p,f[3],k=0,vector[3]={0,0,0};


void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[])
{
p=atoi(argv[1]);
 PORT[1] = atoi(argv[2]);
 PORT[2] = atoi(argv[3]);
 PORT[3] = atoi(argv[4]);
  server = gethostbyname(argv[5]);
pthread_t thread1, thread2;


pthread_create(&thread1 , NULL ,  &serverf ,&PORT[p]);


sleep(5);

  pthread_create(&thread2 , NULL ,  &client,NULL);


 pthread_join(thread1, NULL);

 pthread_join(thread2, NULL);

return 0;
}

//multicasting messages

void *multicast(void *arg)
{
int *temp= (int*) arg;
        int portno =*temp;

int sockfd, newsockfd;
     socklen_t clilen;
   char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
    
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

vector[p-1]=vector[p-1]+1;
//printf("yes");
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

sleep(5);
     listen(sockfd,5);

     clilen = sizeof(cli_addr);
        // printf("Please enter the message: ");
   bzero(buffer,256);
    fgets(buffer,255,stdin);

while(1)
{
   newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
  
sleep(5);
 
for(int j=0;j<3;j++)
{
printf("%d vc ",vector[j]);

}
    n=write(newsockfd,&buffer,sizeof(buffer));
    n = write(newsockfd,&vector,sizeof(vector));
    if (n < 0)
         error("ERROR writing to socket");
}
}
void *listerner(void *arg)
   {

         int sockfd, portno, n;
    struct sockaddr_in serv_addr;
  	int i;
        for(i=1;i<=3;i++)
        {

        if(i!=p)
        {

        portno=PORT[i];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
	sleep(8);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
	int buff[3]={0,0,0};
	printf("p: %d\n",p);
	sleep(8);
	char rbuffer[256];
   n=read(sockfd,&rbuffer,sizeof(rbuffer));
   n = read(sockfd,&buff,sizeof(buff));
    if (n < 0) error("ERROR reading from socket");
	for(int i=0;i<3;i++)
	{
	if(buff[i]>vector[i])
	{
	vector[i]=buff[i];
	}
	}
	for(int i=0;i<3;i++)
	{
	   printf("Here is the vector clock: %d\n",vector[i]);
	}
	printf("here is the message:%s\n",rbuffer);

	}
	}
	}

