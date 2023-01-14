#include "../header/chase.h"

int NPlayers, NBots;
playerList_t *listInit;

void* thread_10secs(void* arg){
	thread_args_t *args= (thread_args_t*) arg;
	player_t* myPlayer = args->bots;
	message_s2c_t message_to_send;
	playerList_t* myPlayer_inList=findInList(args->list_of_players, myPlayer->position.c);
	
	printf("Comecei a thread dos 10 segundos \n");
	sleep(10);

	if ( myPlayer_inList->is_active== 0) {
		
		printf("Passaram 10s sem que nada acontecesse\n");
		message_to_send.type = -3;
		message_to_send.player_dummy = *myPlayer;
		send_msg_through_list(args->list_of_players, message_to_send);
		close(args->self_client_fd);
		RemoveFromList(args->list_of_players, *myPlayer);
	}
	free(arg);
	pthread_exit(NULL);
	
}


void* thread_players(void* arg){
	thread_args_t *args= (thread_args_t*) arg;
	thread_args_t *args10s;
	int self_client_connection=args->self_client_fd;
	playerList_t* aux, *myPlayer;
	playerList_t newplayer;
	player_t dummy_player;
	message_s2c_t message_to_send;
	message_c2s_t message_from_client;

	if(NPlayers>= MAX_CLIENTS){
		message_to_send.type=-1;
		send(self_client_connection, &message_to_send, sizeof(message_s2c_t),0);
		return NULL;
	}
	else{
		NPlayers++;
	}

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
	newplayer.is_active = 1;

	printf("2\n");
	// message_to_send.type = 1;
	// for( aux = args->list_of_players; aux->next!= NULL; aux = aux->next) {

	// 	message_to_send.player_dummy = aux->next->player;
	// 	write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
	// }
	printf("3\n");

	//Transmits bots info
	message_to_send.type = 3;
	memcpy(message_to_send.bots, args->bots, 10*sizeof(player_t));
	write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
	printf("4\n");

	//transmits rewards info
	message_to_send.type = 4;
	memcpy(message_to_send.rewards, args->rewards, 10*sizeof(player_t));
	write(self_client_connection, &message_to_send, sizeof(message_s2c_t));

	//transmits the ball info to the client 
	message_to_send.player_dummy = newplayer.player;
	message_to_send.type = 0;
	write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
	printf("6\n");

	//receive the confirmation from the player that it started playing
	if (recv(self_client_connection, &message_from_client , sizeof(message_c2s_t), 0) <= 0) {
		perror("Error receiving data from client");
		return NULL;
	}
	printf("7\n");

	//if the message is indeed of that type, adds the player to the list
	if(message_from_client.type == 0){
		//transmits the info that it created a new player to all players
		message_to_send.type=1;
		send_msg_through_list(listInit, message_to_send);
		printf("5\n");
		myPlayer = addToListEnd(args->list_of_players, newplayer);
		for( aux = args->list_of_players; aux->next!= NULL; aux = aux->next) {
			message_to_send.player_dummy = aux->next->player;
			write(self_client_connection, &message_to_send, sizeof(message_s2c_t));
		}
	}
	printf("1º player char: %c \n", args->list_of_players->next->player.position.c);
	printf("8\n");


	//cicle to receive more messages from this client
	while( 1 ){

		if (recv(self_client_connection, &message_from_client , sizeof(message_c2s_t), 0) <= 0) {
			perror("Error receiving data from client");
			return NULL;
		}
		printf("tipo %d %d\n", message_from_client.type, self_client_connection);

		if (message_from_client.type == 1) {
			// À partida se ele consegue enviar esta mensagem, é porque está vivo
			// Se tivesse morto tinha recebido uma mensagem de morte.

			dummy_player = myPlayer->player;
			move_player(&dummy_player.position, message_from_client.direction);
			aux = collision_checker(args->list_of_players, &dummy_player, args->bots, args->rewards, 1, 0);
			message_to_send.type = 2;
			if( aux != NULL) {
				if (aux->player.health == 0)
				{
					aux->is_active = 0;
				}
				
				message_to_send.player_dummy = aux->player;
				send_msg_through_list(listInit, message_to_send);
			}
			else {
				// Não bateu contra um player
				myPlayer->player = dummy_player;
				message_to_send.player_dummy = dummy_player;
				send_msg_through_list(args->list_of_players, message_to_send);
				// function to write to everyone, not sure se ele fica com o write de cima.
			}
		}
		if (message_from_client.type == -1) {
			// Player has health == 0!
			printf("Player %c com saúde a 0\n", myPlayer->player.position.c);
			args10s = malloc(sizeof(thread_args_t));
			args10s->self_client_fd=args->self_client_fd;
			args10s->list_of_players = listInit;
			args10s->bots = &myPlayer->player; // Usar bots para passar player
			if(pthread_create(&(args10s->self_thread_id), NULL, thread_10secs, (void *)args10s)!=0){
			perror("Error while creating Thread");
			}

			if (recv(self_client_connection, &message_from_client , sizeof(message_c2s_t), 0) <= 0) {
				perror("Error receiving data from client");
				pthread_exit(NULL);
			}
			printf("tipo %d %d\n", message_from_client.type, self_client_connection);
			if (message_from_client.type == -1){
				// Client wants to die
				printf("Cliente %c wants to die\n", myPlayer->player.position.c);
				myPlayer->is_active = -1; 
				message_to_send.type = -3;
				message_to_send.player_dummy = myPlayer->player;
				send_msg_through_list(args->list_of_players, message_to_send);
				close(args->self_client_fd);
				RemoveFromList(args->list_of_players, myPlayer->player);
				pthread_exit(NULL);		
			}
			if (message_from_client.type == 2) {
				// Client wants to keep playing
				printf("Client %c wants to keep playing\n", myPlayer->player.position.c);
				myPlayer->is_active = 1;
				myPlayer->player.health = 10;
				message_to_send.type = -2;
				send(self_client_connection, &message_to_send, sizeof(message_s2c_t),0);
				message_to_send.type=2;
				message_to_send.player_dummy=myPlayer->player;
				send_msg_through_list(listInit, message_to_send);		
			}
			
		}
		
	}
    // Close the connection
    close(self_client_connection);
    free(args);
	NPlayers--;
    return NULL;
}

void* thread_rewards(void* arg){
	thread_args_t *args= (thread_args_t*) arg;
	message_s2c_t message_with_rewards;
	int i,x,y;
	//create first five rewards
	init_rewards_board(args->rewards, args->bots, args->list_of_players);
	message_with_rewards.type=4;
	
	while(1){
		sleep(5);
		for(i=0;i<10;i++){
			if(args->rewards[i].flag==0){
				args->rewards[i].flag=1;
				args->rewards[i].value= RandInt(1,5);
				do{
                    x=RandInt(1,WINDOW_SIZE-2);
                    y=RandInt(1,WINDOW_SIZE-2);
                }while(is_free_position(args->rewards, args->bots, args->list_of_players, x, y)==false);
				args->rewards[i].x=x;
                args->rewards[i].y=y;
				memcpy(message_with_rewards.rewards, args->rewards, 10*sizeof(reward_t));
				send_msg_through_list(args->list_of_players, message_with_rewards);
				break;
			}
		}
	}

}

void* thread_bots(void* arg){
	thread_args_t *args= (thread_args_t*) arg;
	int nr_bots=args->self_client_fd; //use the int to just pass this information instead
	player_t* bots=args->bots;
	playerList_t* listInit=args->list_of_players;
	reward_t* rewards=args->rewards;
	player_t player_dummy;
	message_s2c_t message_with_bots;

	int i,x,y;
	init_bots_health(bots);
	message_with_bots.type=3;

	for(i=0; i<nr_bots;i++){
		bots[i].health=10;
		bots[i].position.c='*';
		do{
			x=RandInt(1,WINDOW_SIZE-1);
			y=RandInt(1,WINDOW_SIZE-1);
			
		}while (!(is_free_position(rewards, bots, listInit ,x,y)));
		bots[i].position.x=x;
		bots[i].position.y=y;	
	}

	while(1){
		sleep(3);
		for(i=0;i<nr_bots;i++){
			player_dummy=bots[i];
			move_player(&player_dummy.position, RandInt(KEY_DOWN,KEY_RIGHT));
			collision_checker(listInit, &player_dummy, bots, rewards, false, i);
			bots[i].position.x=player_dummy.position.x;
			bots[i].position.y=player_dummy.position.y;
		}
		memcpy(message_with_bots.bots, bots, 10*sizeof(player_t));
		send_msg_through_list(listInit, message_with_bots);
	}
}

int main(int argc, char *argv[]){
	player_t bots[NBOTS];//max possible size of vector
	reward_t rewards[10];

	int nr_bots=atoi(argv[argc-1]);

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


	thread_args_t *args;
	listen(sock_fd,5);

	// launching rewards thread
	args = malloc(sizeof(thread_args_t));
    // args->self_client_fd=argv[argc-1];
	args->list_of_players = listInit;
	args->bots = bots;
	args->rewards = rewards;
	if(pthread_create(&(args->self_thread_id), NULL, thread_rewards, (void *)args)!=0){
		perror("Error while creating Thread");
	}
	printf("Criei uma thread|! \n");

	// launching bots thread
	args = malloc(sizeof(thread_args_t));
    args->self_client_fd=nr_bots;
	args->list_of_players = listInit;
	args->bots = bots;
	args->rewards = rewards;
	if(pthread_create(&(args->self_thread_id), NULL, thread_bots, (void *)args)!=0){
		perror("Error while creating Thread");
	}
	printf("Criei uma thread|! \n");

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