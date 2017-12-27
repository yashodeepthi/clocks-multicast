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
#include <stack>


void *multicast(void *);
void *listerner(void *);
struct hostent *server;
int PORT[3],sum=0,p,f[3],k=0;
pthread_mutex_t mutex;

struct msgbuf
{
int vector[3]={0,0,0};
char buffer[256];
int buff[3];
int pid;
}smsg,tmsg;

std::stack<msgbuf> cqueue;
std::stack<msgbuf> squeue;


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


pthread_create(&thread1 , NULL ,  &multicast ,&PORT[p]);


sleep(5);

  pthread_create(&thread2 , NULL ,  &listerner,NULL);


 pthread_join(thread1, NULL);

 pthread_join(thread2, NULL);

return 0;
}
void *multicast(void *arg)
{
int *temp= (int*) arg;
        int portno =*temp;
struct msgbuf sbuf;
int sockfd, newsockfd;
     socklen_t clilen;
 
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");


     listen(sockfd,5);

     clilen = sizeof(cli_addr);
sleep(5);
		pthread_mutex_lock(&mutex);
        sbuf.vector[p-1]=sbuf.vector[p-1]+1;
        pthread_mutex_unlock(&mutex);
       
while(1)
{
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
  
sleep(8);
 sbuf.pid=p;
    n = write(newsockfd,&sbuf,sizeof(sbuf));
    if (n < 0)
         error("ERROR writing to socket");


}
}
void *listerner(void *arg)
         {

         int sockfd, portno, n;
    struct sockaddr_in serv_addr;

struct msgbuf rbuff;
  
    
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



printf("p: %d\n",p);
   n = read(sockfd,&rbuff,sizeof(rbuff));
    if (n < 0) error("ERROR reading from socket");

printf("%d /n",rbuff.pid);
// buffering 

 if(cqueue.empty())
        cqueue.push(rbuff);
   else{

        tmsg=cqueue.top();
		cqueue.pop();
        int spid=rbuff.pid;
        if(tmsg.vector[spid-1]+1==rbuff.vector[spid-1])
        {
          if(spid==3){
           if((tmsg.vector[spid-3]<=rbuff.vector[spid-3])&& (tmsg.vector[spid-2]<=rbuff.vector[spid-2])){

            tmsg.vector[spid-1]=rbuff.vector[spid-1];
           cqueue.push(tmsg);
          printf("message delivered");
}
}
if(spid==2){
         if((tmsg.vector[spid-2]<=rbuff.vector[spid-2])&& (tmsg.vector[spid]<=rbuff.vector[spid])){
           tmsg.vector[spid-1]=rbuff.vector[spid-1];
           cqueue.push(tmsg);
          printf("message delivered");
}

}
if(spid==1){

 if((tmsg.vector[spid]<=rbuff.vector[spid])&& (tmsg.vector[spid+1]<=rbuff.vector[spid+1])){

                         tmsg.vector[spid-1]=rbuff.vector[spid-1];
                        cqueue.push(tmsg);
}
}
}
else{

    squeue.push(rbuff);
}
}

for(int i=0;i<3;i++)
{
    printf("Here is the message: %d\n",rbuff.vector[i]);
}
}
}
}
