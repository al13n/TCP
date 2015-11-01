#include "unpthread.h"
#include <fcntl.h>
#include <sys/file.h>
#define ECHO_PORT 16667
#define TIME_PORT 16668


static void *echoServer(void *);
static void *timeServer(void *);
int bindListenAtPort(int);

int main(){

	int listenfd_echo;
	int listenfd_time;
	int maxfdp1;
	char buff[MAXLINE];
	pthread_t tid;

	listenfd_echo =  bindListenAtPort(ECHO_PORT);
	listenfd_time = bindListenAtPort(TIME_PORT);

	for ( ; ; ) {
		fd_set rset;	
		maxfdp1 = max(listenfd_time, listenfd_echo) + 1;

		FD_ZERO(&rset);
		FD_SET(listenfd_echo, &rset);
		FD_SET(listenfd_time, &rset);

		Select(maxfdp1, &rset, NULL, NULL, NULL);
		

		if( FD_ISSET(listenfd_echo, &rset))
		{	
			int connfd = Accept(listenfd_echo, (SA *) NULL, NULL);
			Pthread_create(&tid, NULL, &echoServer, (void *)connfd);
		}

		if( FD_ISSET(listenfd_time, &rset))
		{
			int connfd = Accept(listenfd_time, (SA *) NULL, NULL);
			Pthread_create(&tid, NULL, &timeServer, (void *)connfd);
		}
		
	}
	return 0;
}

int bindListenAtPort(int portno){
	int fileflags, option_value;
	struct sockaddr_in	servaddr;
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	if (fileflags = fcntl(sockfd, F_GETFL, 0) == -1) {
	 perror("fcntl F_GETFL");
	 exit(1);
	}
	if (fcntl(sockfd, F_SETFL, fileflags | FNDELAY) == -1) {
	 perror("fcntl F_SETFL, FNDELAY");
	 exit(1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&option_value, sizeof(option_value)) < 0) {
		fprintf(stderr, "setsockopt failure\n");
		exit(1);
  	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(portno);

	Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(sockfd, LISTENQ);
	
	return sockfd;
}

static void * echoServer(void *arg){
	char buff[MAXLINE];
	int n;
	fd_set rset;

	Pthread_detach(pthread_self());
	
	for(;;){
		FD_ZERO(&rset);
		FD_SET((int)arg, &rset);

		Select((int)arg + 1, &rset, NULL, NULL, NULL);
		if(FD_ISSET((int)arg, &rset)){
			if((n = Readline((int)arg, buff, MAXLINE)) > 0){
				Writen((int)arg, buff, strlen(buff));
			}	
			if( n == 0){
				break;
			}
		}
		
	}
		
	Close((int) arg);
	return (NULL);
}


static void * timeServer(void *arg){

	Pthread_detach(pthread_self());

	struct timeval t_sleep;
	fd_set rset;
	char buff[MAXLINE];
	time_t ticks;

	for(;;){
		t_sleep.tv_sec = 5;
		FD_ZERO(&rset);
		FD_SET((int)arg, &rset);

		Select((int)arg + 1, &rset, NULL, NULL, &t_sleep);
		
		if( FD_ISSET((int)arg, &rset)) {
			if(read(arg, buff, MAXLINE) == 0){
				printf("Time child terminated\n");
				break;
			}
		}
		else{
			ticks = time(NULL);
        	snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        	Write((int)arg, buff, strlen(buff));
		}
	}
	
	Close((int) arg);
	return (NULL);
}


