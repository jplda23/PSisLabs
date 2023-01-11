#include "../header/chase.h"

int NPlayers, NBots;
player_t players[((WINDOW_SIZE-1)*(WINDOW_SIZE-1)/9)]; //Assuming 0 Bots and 0 prizes, max possible size of vector
player_t bots[NBOTS];//max possible size of vector
reward rewards[10];
playerList_t *listInit;




void* thread_players(void* arg){
	thread_args_t *args= (thread_args_t*) arg;
	int self_client_connection=args->self_client_fd;
	
	char buffer[1024];


	while( 1 ){

		int bytes_received = recv(self_client_connection, buffer, sizeof(buffer), 0);
		if (bytes_received <= 0) {
			perror("Error receiving data from client");
			exit(EXIT_FAILURE);
		}

		// Print the received data
		printf("Received %d bytes: %s from %d\n", bytes_received, buffer, self_client_connection);
	}
    // Close the connection
    close(self_client_connection);
    free(arg);

    return NULL;
}

int main(int argc, char *argv[]){

	//Initialize list of players with first element being a void element
	listInit = (playerList_t*) calloc(1,sizeof(playerList_t));
	if (listInit==NULL){
		perror("Erro na alocação de memória");
		return -1;
	}
	listInit->player.position.c='\0';
	listInit->next=NULL;


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

    int nbytes, check;
    char buffer[100];
    char remote_addr_str[100];
	thread_args_t *args;
	listen(sock_fd,5);

    while(1){
		args = malloc(sizeof(thread_args_t));
        args->self_client_fd=accept(sock_fd, NULL, NULL);
		if(pthread_create(&(args->self_thread_id), NULL, thread_players, (void *)args)!=0){
			perror("Error while creating Thread");
		}

        // nbytes = recvfrom(sock_fd, buffer, 100, 0,
		//                   ( struct sockaddr *)&client_addr, &client_addr_size);
        // nbytes= recv(client_fd,buffer, sizeof(buffer),0);
		// if(nbytes<0){
        //     perror("ERROR receiving data fro socket");
        // }

		//printf("received %d bytes from %s \n", nbytes,buffer);
		

    }
}