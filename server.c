#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

struct client_info {
	int sockno;
	char ip[INET_ADDRSTRLEN];
};

int clients[100];
int n = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Sends the message to all clients.
void sendToAll(char* msg, int curr)
{
	int i;
	pthread_mutex_lock(&mutex);
	
	for (i = 0; i < n; i++) 
	{
		if (clients[i] != curr) 
		{
			if (send(clients[i], msg, strlen(msg), 0) < 0) 
			{
				perror("sending failure...");
				continue;
			}
		}
	}

	pthread_mutex_unlock(&mutex);
}

// Receives a message from a client.
void* recvMsg(void* sock)
{
	struct client_info cl = *((struct client_info*)sock);
	char msg[500];
	int len;
	int i;
	int j;
	
	while ((len = recv(cl.sockno, msg, 500, 0)) > 0) 
	{
		msg[len] = '\0';
		sendToAll(msg, cl.sockno);
		memset(msg, '\0', sizeof(msg));
	}
	
	pthread_mutex_lock(&mutex);

	printf("%s disconnected...\n", cl.ip);
	
	for (i = 0; i < n; i++) 
	{
		if (clients[i] == cl.sockno) 
		{
			j = i;
		
			while (j < (n - 1)) 
			{
				clients[j] = clients[j + 1];
				j++;
			}
		}
	}

	n--;
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char* argv[])
{
	struct sockaddr_in my_addr, their_addr;
	struct client_info cl;
	socklen_t their_addr_size;
	pthread_t sendt, recvt;
	int my_sock;
	int their_sock;
	int portno;
	int len;
	char msg[500];
	char ip[INET_ADDRSTRLEN];

	if (argc > 2) 
	{
		perror("too many arguments...");
		exit(1);
	}
	
	portno = atoi(argv[1]);
	my_sock = socket(AF_INET, SOCK_STREAM, 0);														// Creates socket.
	if (my_sock < 0)																				// Validate the socket.
	{
		perror("opening channel unsuccessful...");
		exit(1);
	}

	memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
	my_addr.sin_family = AF_INET;																	// Sets IPv4.
	my_addr.sin_port = htons(portno);																// Sets port number.
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");												// Sets IP address.
	their_addr_size = sizeof(their_addr);

	if (bind(my_sock, (struct sockaddr*) & my_addr, sizeof(my_addr)) != 0)							// Binds the socket with the specific IP + port.
	{
		perror("binding unsuccessful...");
		exit(1);
	}

	if (listen(my_sock, 5) != 0)																	// Listens for connection request.
	{
		perror("listening unsuccessful...");
		exit(1);
	}

	while (1) 
	{
		if ((their_sock = accept(my_sock, (struct sockaddr*) & their_addr, &their_addr_size)) < 0)	// Accepts a connection request - blocking call.
		{
			perror("accept unsuccessful...");
			exit(1);
		}
	
		pthread_mutex_lock(&mutex);
		inet_ntop(AF_INET, (struct sockaddr*) & their_addr, ip, INET_ADDRSTRLEN);
		
		printf("%s connected...\n", ip);

		cl.sockno = their_sock;
		strcpy(cl.ip, ip);
		clients[n] = their_sock;
		n++;
		
		pthread_create(&recvt, NULL, recvMsg, &cl);
		pthread_mutex_unlock(&mutex);
	}

	return 0;
}