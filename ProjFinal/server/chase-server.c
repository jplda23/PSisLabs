#include "../header/chase.h"

int NPlayers, NBots;
player_t bots[NBOTS];//max possible size of vector
reward_t rewards[10];
playerList_t *listInit;




void* thread_players(void* arg){
	thread_args_t *args= (thread_args_t*) arg;
	int self_client_connection=args->self_client_fd;
	int bytes_received;
	playerList_t* aux, *myPlayer;
	playerList_t newplayer;
	player_t dummy_player;
	message_s2c_t message_to_send;
	message_c2s_t message_from_client;

	printf("Entrei na fucking thread \n");

	do{
		new_player(&newplayer.player.position, args->list_of_players, args->bots, args->rewards, RandInt('A','Z'));
		printf("Cheguei ao new_player\n");
	}while(already_existent_char(args->list_of_players, newplayer.player.position.c)!=0);// cycle
	printf("char: %c \n", newplayer.player.position.c);
	printf("Position x: %d \n", newplayer.player.position.x);
	printf("Position y: %d \n", newplayer.player.position.y);
	printf("1\n");
	newplayer.thread_player = args->self_thread_id;
	newplayer.client_fd_player = args->self_client_fd;
	newplayer.player.health = 10;

	printf("2\n");
	message_to_send.type = 1;
	for( aux = args->list_of_players; aux->next!= NULL; aux = aux->next) {

		message_to_send.player_dummy = aux->next->player;
		write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
	}
	printf("3\n");
	message_to_send.type = 3;
	memcpy(message_to_send.bots, args->bots, 10*sizeof(player_t));
	write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
	printf("4\n");
	message_to_send.type = 4;
	memcpy(message_to_send.rewards, args->rewards, 10*sizeof(player_t));
	write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
	printf("5\n");
	message_to_send.type = 0;
	message_to_send.player_dummy = newplayer.player;
	write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
	printf("6\n");
	if (recv(self_client_connection, &message_from_client , sizeof(message_c2s_t), 0) <= 0) {
		perror("Error receiving data from client");
		exit(EXIT_FAILURE);
	}
	printf("7\n");
	if(message_from_client.type == 0)
		myPlayer = addToListEnd(args->list_of_players, newplayer);
	printf("1º player char: %c \n", args->list_of_players->next->player.position.c);
	printf("8\n");

	while( 1 ){

		if (recv(self_client_connection, &message_from_client , sizeof(message_c2s_t), 0) <= 0) {
			perror("Error receiving data from client");
			exit(EXIT_FAILURE);
		}

		if (message_from_client.type == 1) {
			// À partida se ele consegue enviar esta mensagem, é porque está vivo
			// Se tivesse morto tinha recebido uma mensagem de morte.

			dummy_player = myPlayer->player;
			move_player(&dummy_player.position, message_from_client.direction);
			aux = collision_checker(args->list_of_players, &dummy_player, args->bots, args->rewards, 1, 0);
			if( aux != NULL) {
				// bateu contra um player
				if (aux->player.health == 0)
				{
					// Kill player
				}
			}
			else {
				// Não bateu contra um player
				myPlayer->player = dummy_player;
				message_to_send.type = 2;
				message_to_send.player_dummy = dummy_player;
				write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
			}


		}
		
	}
    // Close the connection
    close(self_client_connection);
    free(args);

    return NULL;
}

void* thread_rewards(void* arg){
	thread_args_t *args= (thread_args_t*) arg;
	int i,x,y;
	//create first five rewards
	init_rewards_board(args->rewards, args->bots, args->list_of_players);

	while(1){
		usleep(5);
		for(i=0;i<10;i++){
			if(args->rewards[i].flag==0){
				args->rewards[i].flag=1;
				args->rewards[i].value= RandInt(1,5);
				do{
                    x=RandInt(1,WINDOW_SIZE-2);
                    y=RandInt(1,WINDOW_SIZE-2);
                }while(is_free_position(rewards,bots, args->list_of_players, x, y)==false);
				args->rewards[i].x=x;
                args->rewards[i].y=y;
				// function to send to all players
				break;
			}
		}
	}

}
//  void* thread_bots(void* arg){
// 	thread_args_t *args= (thread_args_t*) arg;
//  }

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
		args->list_of_players = listInit;
		args->bots = bots;
		args->rewards = rewards;
		if(pthread_create(&(args->self_thread_id), NULL, thread_players, (void *)args)!=0){
			perror("Error while creating Thread");
		}
		printf("Criei uma thread|! \n");

        // nbytes = recvfrom(sock_fd, buffer, 100, 0,
		//                   ( struct sockaddr *)&client_addr, &client_addr_size);
        // nbytes= recv(client_fd,buffer, sizeof(buffer),0);
		// if(nbytes<0){
        //     perror("ERROR receiving data fro socket");
        // }

		//printf("received %d bytes from %s \n", nbytes,buffer);
		

    }
}