#include "../header/chase.h"

int main(int argc, char *argv[]){

    char* socket_port = argv[argc-1];
    char* socket_address = argv[argc-2];


    int sock_fd= socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	printf(" socket created \n Ready to send\n");

    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(socket_port));
	if( inet_pton(AF_INET, socket_address, &server_addr.sin_addr) < 1){
		printf("no valid address: \n");
		exit(-1);
	}

}