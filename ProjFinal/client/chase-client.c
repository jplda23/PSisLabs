
#include "../header/chase.h"

playerList_t My_player;
WINDOW * my_win;
WINDOW* message_win;
int global_player_will_die=0;

void* thread_listenner(void* arg){
	thread_args_t * args = (thread_args_t*) arg;
	int sock_fd= args->self_client_fd;
	playerList_t* listInnit= args->list_of_players, *dummy_pointer;
	playerList_t dummy_player;
	message_s2c_t message_received;
	message_c2s_t message_to_server;
	int nbytes;

	dummy_player.client_fd_player=0;
	dummy_player.thread_player=0;
	while(1){
		nbytes=recv(sock_fd, &message_received , sizeof(message_s2c_t), 0);
		if(nbytes<0){
			perror("Error receiving message from server");
			return NULL;
		}
		else{
			switch(message_received.type){
				case -3:
					if (message_received.player_dummy.position.c == My_player.player.position.c) {
						global_player_will_die=2;
						close(sock_fd);
						werase(my_win);
						mvwprintw(my_win, 2, 1, "You are out!\n");
						mvwprintw(my_win, 4, 1, "Press any key to exit!\n");
						wrefresh(my_win);
						pthread_exit(NULL);
					}
					else {

						RemoveFromList(args->list_of_players, message_received.player_dummy);
						delete_and_draw_board(my_win, message_win, listInnit,  args->bots,  args->rewards);
					}
					break;	

				case -2: //caso de voltar ao jogo
					global_player_will_die=0;
					dummy_pointer=findInList(listInnit, message_received.player_dummy.position.c);
					if(dummy_pointer!=NULL){
						dummy_pointer->player=message_received.player_dummy;
					}
					delete_and_draw_board(my_win, message_win, listInnit,  args->bots,  args->rewards);
					break;
				case -1:
					printf("not possible to play, to many players"); //avisar que server is at capacity
					exit(-1);
					break;
				
				case 0:
					My_player.player =message_received.player_dummy;
					My_player.thread_player=0;
					My_player.client_fd_player=0;
					addToListEnd(listInnit, My_player);
					printf("Your character is %c press Enter to continue\n", My_player.player.position.c);
					getchar();

					//INIT NCURSES

    				initscr();		    	/* Start curses mode 		*/
					cbreak();				/* Line buffering disabled	*/
    				keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
					noecho();			    /* Don't echo() while we do getch */

    				/* creates a window and draws a border */
    				my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    				keypad(my_win, true);
    				message_win = newwin(10, WINDOW_SIZE, WINDOW_SIZE, 0);


					message_to_server.type=0;
					send(sock_fd, &message_to_server, sizeof(message_c2s_t),0);
					delete_and_draw_board(my_win, message_win, listInnit,  args->bots,  args->rewards);
					break;
				
				case 1:
					dummy_player.player=message_received.player_dummy;
					if(findInList(listInnit, dummy_player.player.position.c)==NULL){
						addToListEnd(listInnit, dummy_player);
					}
					if( global_player_will_die==0){
						delete_and_draw_board(my_win, message_win, listInnit,  args->bots,  args->rewards);
					}
					break;
				
				case 2:
					dummy_pointer=findInList(listInnit, message_received.player_dummy.position.c);
					if(dummy_pointer!=NULL){
						dummy_pointer->player=message_received.player_dummy;
						if(dummy_pointer->player.position.c==My_player.player.position.c){
							if(dummy_pointer->player.health==0){
								global_player_will_die=1;
								werase(my_win);
								werase(message_win);
								mvwprintw(my_win, 2, 1, "Do You Wish to Continue playing?\n");
								mvwprintw(my_win, 3, 1, "You have 10 seconds to decide!\n");
								mvwprintw(my_win, 4, 1, "Press C to continue, Q to exit!\n");
								wmove(my_win, 5, 1);
								wrefresh(my_win);
								wrefresh(message_win);
							}
						}
						if( global_player_will_die==0){
							delete_and_draw_board(my_win, message_win, listInnit,  args->bots,  args->rewards);
						}
					}
					break;

				case 3:
					memcpy(args->bots, &message_received.bots, 10*sizeof(player_t));
					if( global_player_will_die==0){
						delete_and_draw_board(my_win, message_win, listInnit,  args->bots,  args->rewards);
					}
					break;

				case 4:
					memcpy(args->rewards, &message_received.rewards, 10*sizeof(reward_t));
					if( global_player_will_die==0){
						delete_and_draw_board(my_win, message_win, listInnit,  args->bots,  args->rewards);
					}
					break;

			}
		}
	}

}

int main(int argc, char *argv[]){

    char* socket_port = argv[argc-1];
    char* socket_address = argv[argc-2];
	playerList_t* listInnit, *aux;
	listInnit= malloc(sizeof(playerList_t));
	listInnit->next=NULL;
	reward_t rewards[10];
	player_t bots[10];
	message_c2s_t message_to_server;


    int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

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

	thread_args_t args;
	args.self_client_fd=sock_fd;
	args.list_of_players=listInnit;
	args.bots=bots;
	args.rewards=rewards;

	if(pthread_create(&(args.self_thread_id), NULL, thread_listenner, (void *)&args)!=0){
			perror("Error while creating Thread");
		}

	int key = -1;
    while( key != 27 && key!= 'q'){

		key = wgetch(my_win);
		if (global_player_will_die == 0)
		{
			if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
				message_to_server.type=1;
				message_to_server.direction=key;
				sendto(sock_fd, &message_to_server, sizeof(message_c2s_t), 0, 
				(const struct sockaddr *)&server_addr, sizeof(server_addr));
        	}
		}
		if (key == 'c')
		{
			//send message to server to keep playing
			message_to_server.type = 2;
			global_player_will_die = 0;
			send(sock_fd, &message_to_server, sizeof(message_c2s_t),0);
			// aqui depois de mandar a msg para o server o client recebe uma msg do server de reiniciar o jogo (caso -2), e recomeça o jogo			
		}
		if(global_player_will_die==2){
			break;
		}
	}	
    
	werase(my_win);
    werase(message_win);
    wrefresh(message_win);
    if(key=='q'){
        message_to_server.type=-1; //disconnect type
        sendto(sock_fd, &message_to_server, sizeof(message_c2s_t), 0, 
                    (const struct sockaddr *)&server_addr, sizeof(server_addr));
    }
    mvwprintw(my_win, 2, 1, "Good Game!\n");
    wmove(my_win,3, 1);
    wrefresh(my_win);
	close(sock_fd);
	aux=listInnit;
	while(aux->next!=NULL){
		listInnit=aux->next;
		free(aux);
		aux=listInnit;
	}

	exit(0);
}