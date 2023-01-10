#include "../header/chase.h"

int NPlayers, NBots;
player players[((WINDOW_SIZE-1)*(WINDOW_SIZE-1)/9)]; //Assuming 0 Bots and 0 prizes, max possible size of vector
player bots[NBOTS];//max possible size of vector
reward rewards[10];


// void* thread_prizes(void* thread_arg){
//     board= (*struct Board)* thread_arg;
//     init_rewards_board(rewards, players, bots);
// }

int main(int argc, char *argv[]){

    int sock_fd;
	sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

    char* socket_port = argv[argc-2];
    char* socket_address = argv[argc-3];

	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(atoi(socket_port));
    inet_pton(AF_INET, socket_address, &local_addr.sin_addr.s_addr);
	// local_addr.sin_addr.s_addr = socket_address;



	int err = bind(sock_fd, (struct sockaddr *)&local_addr,
							sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}



	printf(" socket created and binded \n ");
	printf("Ready to receive messages\n");

    int nbytes;
    char buffer[100];
    char remote_addr_str[100];
    int client_fd;

    while(1){   

        listen(sock_fd,5);
        client_fd=accept(sock_fd, NULL, NULL);

        // nbytes = recvfrom(sock_fd, buffer, 100, 0,
		//                   ( struct sockaddr *)&client_addr, &client_addr_size);
        nbytes= recv(client_fd,buffer, sizeof(buffer),0);
		if(nbytes<0){
            perror("ERROR receiving data fro socket");
        }

		printf("received %d bytes from %s \n", nbytes,buffer);
		

    }
}