#Assignment A1:
--------------------------------

System Documentation:
--------------------------------

This is a TCP client and server implementation for the daytime, and echo service. 

Implementation:
---------------------------------

The Daytime service is hardwired to Port# 16667
The Echo service is hardwired to Port# 16668

CLIENT:
---------------------------------

1.Command line arguments procesing: The client starts off by asking an address of the server from the user, which can be given as a domain name or IP address. This has been implemented using the function gethostbyname(), gethostbyaddress() 
where in if the user enters the IP address the domain name is displayed & vice -a-versa.

2.The Client Parent:  Here the client queries the user for the type of service that he would like to choose : echo or daytime. 

3. The Client Child: The main client program forks off and execs a new xterm window where the user interacts with the chosen service. 

4.Service requests termination & programm robustness: The Daytime client is terminated through ^C  which has be handled in the program by notifying server of the terminated child through the SIGCHILD() signal. The ^D and ^C termination of the Echo client has been handled by the use of threads.

5.IPC Pipe: A half duplex pipe has been used for the purpose of communicating status messages from the child to it's parent. 

6.More Robustness: The echo client is handled by 2 separate threads that read and write to the server separately which adds robustness and handles SIGCHLD efficiently.

SERVER:
----------------------------------

1.Multithreading services: Using a select call to listen for the daytime and echo service.When a connection arrives to a particular listening port, separate threads have been used to service multiple clients, similar to a inetd superserver.

2.Relation to Inetd superserver: This is implemented somewhat in line to how a inetd superserver listens for the various services, and handles it separately.

3. Listening on Multiple Services: A select() call is made to handle listening services, and they have been  binded to non-blocking sockets to make the program robust.

4.Robustness: The use of threads and select makes it possible to identify client behaviour concurrently for read and write. The server is made nonblocking using fcntl , and handles SIGCHLD. 

5.Echo server Implementation: Implemented using a detached thread that services to reads from the client, and uses thread safe functions. 

6.Time server Implementation: Implemented using a select with a 5 sec interval, so that it sleeps. Robustness is added by making a read call on the connected socket, to listen to a FIN packet from the client. 

7.Proper status message: Implemented on the server to 

8.SO_REUSEADDR socket option: Implemented.

9.Non-Blocking concept: Implemented.


User & Testing Documentation:
------------------------------------

Follow these steps to use it:

1.Run the following command, to compile and make executables using the Makefile:
   make

2.Run the executable server using the command:
   ./server &

3.Run the following executable to start your client: 
   ./client <IP Address or Domain name>

   (Eg: ./client 127.0.0.1 
   		./client localhost )

4.Follow the instructions that the client program gives.

5.Clean up object files using: 
   make clean
