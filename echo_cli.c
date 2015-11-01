#include "unpthread.h"
#define SIZE 1024

const int ECHO_SRV_PORT = 16667;

void *copytoServer(void *);
void connectToServer(char *);
void writeToPipe(char *);
void initPipe(int);
void read_str_cli(FILE *);

int sockfd, pipefd;
FILE* fp;
int thread_status;

int main(int argc, char **argv){

	if (argc != 3)
	{
		err_quit("Usage: echo_cli <IPaddress> <fd_PIPE>");
	}

	initPipe(*argv[2]);
	connectToServer(argv[1]);
	read_str_cli(stdin);
			
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

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		snprintf(err_s, sizeof(err_s), "socket error");
		writeToPipe(err_s);
		err_sys(err_s);
	}
		

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


void read_str_cli(FILE *fp_arg)
{
	char		recvline[MAXLINE];
	pthread_t	tid;
	int n;

	fp = fp_arg;
	thread_status = 0;
	Pthread_create(&tid, NULL, copytoServer, NULL);

	while ( (n = Readline(sockfd, recvline, MAXLINE)) >= 0)
	{
		if( n == 0 ){
			if(thread_status)
				return ;
			else{
				writeToPipe("Echo Client: server terminated prematurely");
				err_quit("Echo Client: server terminated prematurely");
			}
				
		}
		Fputs(recvline, stdout);
	}
	return ;
}


void *copytoServer(void *arg)
{
	char	sendline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL)
		Writen(sockfd, sendline, strlen(sendline));

	Shutdown(sockfd, SHUT_WR);
	thread_status = 1;
	return(NULL);
}



