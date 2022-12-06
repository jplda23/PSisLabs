#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
 
#include "sock_dg.h"

int main(){

	char buff[100];
	int nbytes;


	printf("My pid is %d (no other proces has the same pid :)\n", getpid());
	int sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_un local_client_addr;
	local_client_addr.sun_family = AF_UNIX;
	sprintf(local_client_addr.sun_path, "%s_%d", SOCK_ADDRESS, getpid());

	printf("this process address is %s\n", local_client_addr.sun_path);
	unlink(local_client_addr.sun_path);
	int err = bind(sock_fd, (struct sockaddr *)&local_client_addr,
							sizeof(local_client_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

	printf("Socket created \nReady to send\nReady to recieve\n");


	int bytes_message;
	char linha[100];


	char message[100];
	char recv_message[100];


	struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);
	while(1){
		printf("Type the message you want to send to the server ");
		fgets(linha, 100, stdin);
		printf("Going to send a %lu bytes message (it may contain the \\n and \\0 :p \n", strlen(linha)+1);
		nbytes = sendto(sock_fd,
	                    linha, strlen(linha)+1, 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
		printf("\nsent %d %s\n\n", nbytes, buff);

		nbytes = recv(sock_fd, recv_message, 100, 0);
		printf("received %d bytes :\n", nbytes);
		for (int i = 0 ; i < nbytes ; i++){
			printf("%d - %c \n", i, recv_message[i]);
		}
	}
	close(sock_fd);
	exit(0);
}
