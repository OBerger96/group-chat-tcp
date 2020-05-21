#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// Receives a message from a client.
void* recvMsg(void* sock)
{
	int their_sock = *((int*)sock);
	char msg[500];
	int len;

	while ((len = recv(their_sock, msg, 500, 0)) > 0) 
	{
		msg[len] = '\0';
		fputs(msg, stdout);
		memset(msg, '\0', sizeof(msg));
	}
}

int main(int argc, char* argv[])
{
	struct sockaddr_in their_addr;
	pthread_t sendt, recvt;
	int my_sock;
	int their_sock;
	int their_addr_size;
	int portno;
	int len;
	char msg[500];
	char username[100];
	char res[600];
	char ip[INET_ADDRSTRLEN];

	if (argc > 3) 
	{
		printf("too many arguments...");
		exit(1);
	}

	portno = atoi(argv[2]);
	strcpy(username, argv[1]);
	my_sock = socket(AF_INET, SOCK_STREAM, 0);												// Creates client socket. 
	if (my_sock < 0)																		// Validates the socket.
	{
		perror("opening channel unsuccessful...");
		exit(1);
	}

	memset(their_addr.sin_zero, '\0', sizeof(their_addr.sin_zero));
	their_addr.sin_family = AF_INET;														// Sets IPv4.
	their_addr.sin_port = htons(portno);													// Sets port number.
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");									// Sets IP address.

	if (connect(my_sock, (struct sockaddr*) & their_addr, sizeof(their_addr)) < 0)			// Connects to server and Validates the connection.
	{
		perror("connection not esatablished...");
		exit(1);
	}

	inet_ntop(AF_INET, (struct sockaddr*) & their_addr, ip, INET_ADDRSTRLEN);
	printf("connected to %s, start chatting\n", ip);
	pthread_create(&recvt, NULL, recvMsg, &my_sock);
	
	while (fgets(msg, 500, stdin) > 0) 
	{
		strcpy(res, username);
		strcat(res, ": ");
		strcat(res, msg);
	
		len = write(my_sock, res, strlen(res));
		
		if (len < 0) 
		{
			perror("message not sent...");
			exit(1);
		}

		memset(msg, '\0', sizeof(msg));
		memset(res, '\0', sizeof(res));
	}

	pthread_join(recvt, NULL);
	close(my_sock);
}