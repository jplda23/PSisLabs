#include "../header/chase.h"

playerList_t My_player;

void* thread_listenner(void* arg){
	thread_args_t * args = (thread_args_t*) arg;
	int sock_fd= args->self_client_fd;
	playerList_t* listInnit= args->list_of_players, *dummy_pointer;
	playerList_t dummy_player;
	message_s2c_t message_received;
	message_c2s_t message_to_server;

	dummy_player.client_fd_player=0;
	dummy_player.thread_player=0;
	while(1){
		if(recv(sock_fd, &message_received , sizeof(message_c2s_t), 0)<0){
			perror("Error receiving message from server");
		}
		printf("%d",message_received.type);
		switch(message_received.type){
			case -1:
				printf("-1 \n");
				perror("not possible to play, to many players");
				exit(-1);
			
			case 0:
				printf("0\n");
				//probably lock
				My_player.player =message_received.player_dummy;
				My_player.thread_player=0;
				My_player.client_fd_player=0;
				addToListEnd(listInnit, My_player);
				printf("0.1\n");
				printf("Your character is %c press Enter to continue\n", My_player.player.position.c);
				getchar();
				message_to_server.type=0;
				printf("0.2\n");
				send(sock_fd, &message_to_server, sizeof(message_c2s_t),0);
				printf("0.3\n");
				//unlock
			
			case 1:
				printf("1\n");
				dummy_player.player=message_received.player_dummy;
				addToListEnd(listInnit, dummy_player);
			
			case 2:
				printf("2\n");
				dummy_pointer=findInList(listInnit, message_received.player_dummy.position.c);
				dummy_pointer->player=message_received.player_dummy;

			case 3:
				printf("3\n");
				memcpy(args->bots, &message_received.bots, 10*sizeof(player_t));

			case 4:
				printf("4\n");
				memcpy(args->rewards, &message_received.rewards, 10*sizeof(reward_t));

		}
	}

}

int main(int argc, char *argv[]){

    char* socket_port = argv[argc-1];
    char* socket_address = argv[argc-2];
	playerList_t* listInnit;
	listInnit= malloc(sizeof(playerList_t));
	listInnit->next=NULL;
	reward_t rewards[10];
	player_t bots[10];
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
	thread_args_t args;
	args.list_of_players=listInnit;
	args.bots=bots;
	args.rewards=rewards;

	if(pthread_create(&(args.self_thread_id), NULL, thread_listenner, (void *)&args)!=0){
			perror("Error while creating Thread");
		}
	while(1){
		sleep(2);
	}
	// do	
	// {
	// 	fgets(message, 100, stdin);

	// 	nbytes = sendto(sock_fd,
	// 						message, strlen(message)+1, 0,
	// 						(const struct sockaddr *) &server_addr, sizeof(server_addr));

	// 	printf("\nsent %d bytes as %s\n\n", nbytes, message);
	// } while (message[0] != 'q');

	close(sock_fd);

}