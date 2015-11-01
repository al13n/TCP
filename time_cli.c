#include "unpthread.h"
#define SIZE 1024

const int ECHO_SRV_PORT = 16668;

void connectToServer(char *);
void writeToPipe(char *);
void initPipe(int);
void getTime();

int sockfd, pipefd;
FILE* fp;

int main(int argc, char **argv){
	if (argc != 3)
	{
		err_quit("Usage: time_cli <IPaddress> <fd_PIPE>");
	}

	initPipe(*argv[2]);
	connectToServer(argv[1]);
	getTime();

return 0;
}


void initPipe(int fd){
	pipefd = fd;
}

void writeToPipe(char *buf){
 	int n = write(pipefd, buf, strlen(buf)+1);
 	if(n < 0){
 		printf("Err: Error writing to pipe \n");
 	}
}


void connectToServer(char *addr){

	struct sockaddr_in	servaddr;
	char err_s[MAXLINE];

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(ECHO_SRV_PORT);	/* echo server */

	if (inet_pton(AF_INET, addr, &servaddr.sin_addr) <= 0){
		snprintf(err_s, sizeof(err_s), "inet_pton error for %s", addr);
		writeToPipe(err_s);
		err_quit(err_s);
	}
		

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
		snprintf(err_s, sizeof(err_s), "connect error");
		writeToPipe(err_s);
		err_sys(err_s);
	}

	return;	
}


void getTime(){
	char recvline[MAXLINE];
	int n;

	while ( (n = Readline(sockfd, recvline, MAXLINE)) >= 0)
	{
		if( n == 0 ){
			snprintf(recvline, sizeof(recvline), "Server terminated prematurely");
			writeToPipe(recvline);
			return ;
		}
		Fputs(recvline, stdout);
	}
}

