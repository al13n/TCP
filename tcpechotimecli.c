#include "unpthread.h"
#include  <signal.h>

#define SIZE 1024

int sig_flg;
void child_tcpechoclient(char * , FILE* );
void child_tcptimeclient(char * , FILE* );
char * getHost(char *);

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    pid = wait(&stat);
    printf("Child %d terminated\n", pid);
    return ;
}

int main(int argc, char **argv)
{
	int n, choice;
	int pfd[2];
	char buf[MAXLINE + 1];
	char *inet_addr; 

	if( argc != 2)
		err_quit("usage: myclient <hostname>");

	inet_addr = getHost(argv[1]);
	Signal(SIGCHLD, sig_chld);    

	while(1){
		printf("\n\n-------------------------------\n");
		printf("Choose from these options: \n");
	   	printf("1: Connect to TCP Echo Server\n");
	   	printf("2: Connect to TCP Time Server\n");
	   	printf("3: Quit\n");
	   	printf("-------------------------------\n\n");
	   	printf("Enter choice(1-3) : \t");
	   	scanf("%d", &choice);
	   	switch(choice){
	   		case 1: pipe(pfd);
	    			child_tcpechoclient(inet_addr, (FILE *) &pfd[1]); 
	    			close(pfd[1]);
	    			break;
	    	case 2: pipe(pfd);
	    			child_tcptimeclient(inet_addr, (FILE *) &pfd[1]); 
	    			close(pfd[1]);
	    			break;
	    	case 3: printf("Terminating client\n");
	    			exit(0);
	    	default:printf("Enter between 1-3. Try again.\n");
	    			continue;
	    }
	    
	    while ((n = read(pfd[0], buf, SIZE)) > 0)
	    {	
	    	printf("LOG(pipe): Child - %s\n", buf);
	    }
	 		
	 	
	 	close(pfd[0]);

	}

	return 0;
}


char * getHost(char *arg){
	struct in_addr  addr ;
	struct hostent *hp;
	char *inet_addr; 
	char **pp;

	if (inet_aton(arg, &addr) != 0)  {
		hp = gethostbyaddr((const char *)&addr, sizeof(addr), AF_INET); 
	    if(hp == NULL) {
	        err_quit("gethostbyaddr error for host: %s: %s",arg, hstrerror(h_errno));
		}
    }
    else  {                              
		hp = gethostbyname(arg);
    	if (hp == NULL){
   			err_quit("gethostbyname failed for host: %s: %s",arg, hstrerror(h_errno));
		}
	}

    printf("official hostname: %s\n", hp->h_name);

    for (pp = hp->h_aliases; *pp != NULL; pp++)
	printf("alias: %s\n", *pp);

    for (pp = hp->h_addr_list; *pp != NULL; pp++) {
		addr.s_addr = ((struct in_addr *)*pp)->s_addr;
		printf("IPaddress: %s\n", inet_ntoa(addr));
		inet_addr =  inet_ntoa(addr);
    }

    return inet_addr;
}


void child_tcpechoclient(char * addr, FILE* fp){
	pid_t childpid;
	if( (childpid=fork()) == 0)
		execlp("xterm", "xterm", "-e", "./echo_cli", addr, fp, NULL);
}

void child_tcptimeclient(char * addr, FILE* fp){
	pid_t childpid;
	if( (childpid=fork()) == 0)
		execlp("xterm", "xterm", "-e", "./time_cli", addr, fp, NULL);
}