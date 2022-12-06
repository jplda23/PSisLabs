#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "sock_dg.h"

int main(){ 

	int nbytes;


	int sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	printf(" socket created \n Ready to send\n");


	
	char linha[100];
	printf("how many bytes you want to send? ");
	fgets(linha, 100, stdin);
	
	int bytes_message;
	sscanf(linha, "%d", & bytes_message);

	char message[100];
	for ( int i = 0; i <bytes_message; i++){
		message[i] = '0'+i;
	}
	message[bytes_message] = '\0';




	struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);
	while(1){
		printf("press Enter to send a new message\n");
		fgets(linha, 100, stdin);

		nbytes = sendto(sock_fd,
	                    message, bytes_message, 0,
	                    (const struct sockaddr *) &server_addr, sizeof(server_addr));
		printf("sent %d byte of %s\n", nbytes, message);
	}
	close(sock_fd);
	exit(0);
}
