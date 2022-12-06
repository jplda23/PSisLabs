#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "sock_dg.h"
 
int main(){

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
	char linha[100];
	while(1){
		printf("press Enter to receive a new message\n");
		fgets(linha, 100, stdin);
		printf("waiting for 10 bytes\n");
		nbytes = recv(sock_fd, buffer, 10, 0);
		printf("received %d bytes:\n", nbytes);
		for (int i = 0 ; i < nbytes ; i++)
			printf("%d - %c \n", i, buffer[i]);
	}
	close(sock_fd);
	//unlink(SOCK_ADDRESS);
	exit(0);
}
