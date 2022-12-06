#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include "sock_dg.h"
  
int main(){

	char linha[100];

	int sock_fd;
	sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_un local_addr;
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	unlink(SOCK_ADDRESS);
	int err = bind(sock_fd, (struct sockaddr *)&local_addr,
							sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}



	printf(" socket created and binded \n ");
	printf("Ready to receive messages\n");
	char buffer[100];
	int nbytes;


	char reply_message[100];
	struct sockaddr_un client_addr;
	socklen_t client_addr_size = sizeof(struct sockaddr_un);
	while(1){
		printf("press Enter to receive a new message\n");
		fgets(linha, 100, stdin);
		printf("waiting for 100 bytes\n");
		nbytes = recvfrom(sock_fd, buffer, 100, 0,
		                  ( struct sockaddr *)&client_addr, &client_addr_size);
		printf("received %d bytes from %s:\n", nbytes, client_addr.sun_path);
		for (int i = 0 ; i < nbytes ; i++){
			printf("%d - %c \n", i, buffer[i]);
			reply_message[i] = toupper(buffer[i]);
		}
		printf("going to send a reply (press enter to continue\n");
		fgets(linha, 100, stdin);

		nbytes = sendto(sock_fd,
	                    reply_message, strlen(reply_message)+1, 0,
	                    (const struct sockaddr *) &client_addr, client_addr_size);
		printf("\nsent %d %s\n\n", nbytes, reply_message);



	}

	close(sock_fd);
	//unlink(SOCK_ADDRESS);
	exit(0);
}
