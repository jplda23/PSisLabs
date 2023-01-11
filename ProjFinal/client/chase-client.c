#include "../header/chase.h"

int main(int argc, char *argv[]){

    char* socket_port = argv[argc-1];
    char* socket_address = argv[argc-2];

    int nbytes;


    int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	printf(" socket created \n Ready to send\n");

    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(socket_port));
	if( inet_pton(AF_INET, socket_address, &server_addr.sin_addr.s_addr) < 1){
		printf("no valid address: \n");
		exit(-1);
	}

	if(connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))<0){
		perror("ERROR connecting server");
		exit(-1);
	}

    char message[100];

	do	
	{
		fgets(message, 100, stdin);

		nbytes = sendto(sock_fd,
							message, strlen(message)+1, 0,
							(const struct sockaddr *) &server_addr, sizeof(server_addr));

		printf("\nsent %d bytes as %s\n\n", nbytes, message);
	} while (message[0] != 'q')

}