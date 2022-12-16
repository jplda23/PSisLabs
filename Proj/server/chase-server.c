#include "../header/chase.h"

int main(int argc, char *argv[]){

    char* socket_name=argv[argc-1];
    
    //Socket creation and binding
    int sock_fd;
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd == -1){
	    perror("socket: ");
	    exit(-1);
    }
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path, socket_name);

    unlink(socket_name);
    int err = bind(sock_fd, 
            (const struct sockaddr *)&local_addr, sizeof(local_addr));
    if(err == -1) {
	    perror("bind");
	    exit(-1);
    }

    //Variable Creation and initialization

    //Board variables

    player players[10];
    player bots[10];
    reward rewards[10];
    player dummy_player;

    //Board initialization

    init_players_health(players);
    init_bots_health(bots);    
    init_rewards_board(rewards, players, bots);

    //AUX variables

    int n_bytes;
    int array_pos,x, y;
    int i,aux;
    bool bot_conected=false;

    //Sockets variables

    message_s2c message_to_send;
    message_c2s message_received;
    struct sockaddr_un connected_clients[11];//To save all the connected clients and not read from unconnected
    struct sockaddr_un client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_un);
    
    //Time variables

    time_t time_new, time_old;
    time(&time_old);

    //INIT NCURSES

    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    WINDOW * message_win = newwin(10, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);

    //Cycle of the server

    while(1){
        n_bytes = recvfrom(sock_fd, &message_received, sizeof(message_c2s), 0, 
                        (struct sockaddr *)&client_addr, &client_addr_size);
        
        //Disconnects clients who sends wrong message formats and ignores the message
        if (n_bytes!= sizeof(message_c2s)){
            message_to_send.type=2;
            continue;
        }
        if (message_received.type==0){
            //initiate player in array
            array_pos=get_player_input_array_position(players, 10);
            if(array_pos==-1){ //if there is no space available in the server sends a error message
                message_to_send.type=-1;
                sendto(sock_fd, &message_to_send, sizeof(message_s2c), 0, 
                (const struct sockaddr *) &client_addr, client_addr_size);
            }
            else{ // if there is, initiates a player in the server and loads relevant information to the message to send
                players[array_pos].health=10;
                do{
                    new_player(&players[array_pos].position, players, bots, rewards, RandInt('A','Z')); 
                }while(already_existent_char(players,players[array_pos].position.c)!=1);// cycle 

                connected_clients[array_pos]=client_addr;
                message_to_send.type=0;
                memcpy( message_to_send.players,players, 10*sizeof(struct player));
                memcpy( message_to_send.bots,bots, 10*sizeof(struct player));
                message_to_send.array_pos=array_pos;
                message_to_send.id=players[array_pos].position.c;
            }
            
        }
        
        else if (message_received.type == -1) {
            // connect from bot - initialize
            if(!bot_conected){
                connected_clients[10]=client_addr;
                bot_conected=true;
            }
            if(strcmp(connected_clients[10].sun_path,client_addr.sun_path)==0){
                bots[message_received.array_pos].health = 10;
                do{
                    x = RandInt(1, WINDOW_SIZE-2);
                    y = RandInt(1, WINDOW_SIZE-2);
                }while(is_free_position(rewards, bots, players, x, y)==false);
                
                bots[message_received.array_pos].position.x = x;
                bots[message_received.array_pos].position.y = y;
            }
            
                          
        }
        if(message_received.type == 1){
            if(check_key(message_received.direction)==false){
                continue;
            }
            if(check_connection(client_addr, connected_clients)==true){ //IS HUMAN AND WILL UPDATE THE HUMAN, WILL HAVE TO INTRODUCE A CHEATER CHECK TOO
                if (check_cheating(players[array_pos].position.c, message_received.id , client_addr, connected_clients, array_pos)){//check for cheating, only test for humans
                    continue;
                }
                array_pos=message_received.array_pos;
                if (players[array_pos].health==0){//CHECK IF IT WAS KILLED BETWEEN MESSAGES
                    message_to_send.type=2;
                    memcpy( message_to_send.players,players, 10*sizeof(struct player)); //loads info to the message just if the client wants to print the endboard
                    memcpy( message_to_send.bots,bots, 10*sizeof(struct player));
                    message_to_send.array_pos=array_pos;
                    message_to_send.id=dummy_player.position.c;
                    players[array_pos].health=0; //eliminates the player
                    players[array_pos].position.c='1';
                    players[array_pos].position.x=-1;
                    players[array_pos].position.y=-1;
                }
                else{
                    dummy_player=players[array_pos];
                    move_player(&dummy_player.position, message_received.direction);
                    if (collision_checker(players, &dummy_player, bots, rewards, true, array_pos ) == 2) {
                        aux = 0;
                        for (i = 0; i < 10; i++)
                        {
                            if (rewards[i].flag == 1)
                                aux++;
                        }

                        if (aux == 9)
                        {
                            time(&time_old);
                        }
                        
                    } //Check for collision and if collision
                    players[array_pos]=dummy_player;//UPDATE THE POSITION OF THE ACTUAL CLIENT IN THE BOARD
                    message_to_send.type=1;
                    memcpy(message_to_send.players,players, 10*sizeof(struct player));
                    memcpy( message_to_send.bots,bots, 10*sizeof(struct player));
                    message_to_send.array_pos=array_pos;
                    message_to_send.id=dummy_player.position.c;
                }
                
            }
            else if(strcmp(client_addr.sun_path, connected_clients[10].sun_path) == 0 ){ //IS A BOT AND WILL UPDATE THE BOT
                array_pos=message_received.array_pos;
                dummy_player=bots[array_pos];
                move_player(&dummy_player.position, message_received.direction);
                collision_checker(players, &dummy_player, bots, rewards, false, array_pos );//Check for collision and if collision 
                bots[array_pos]=dummy_player;

            }
            
        }

        else if (message_received.type == 2){
            if (check_connection(client_addr, connected_clients)==true){
                if (check_cheating(players[array_pos].position.c, message_received.id , client_addr, connected_clients, array_pos)){
                    continue;
                }
                //DISCONNECT THE CLIENT FROM THE SERVER
                array_pos=message_received.array_pos;
                strcpy(connected_clients[array_pos].sun_path, socket_name);
                players[array_pos].position.c='1';
                players[array_pos].health=0;
                players[array_pos].position.x=-1;
                players[array_pos].position.y=-1;
            }

            
        }

        time(&time_new);
        if(difftime(time_new,time_old)>5.0)//EVERY 5 SECONDS ADDS 1 REWARD(if less than 10)

        {
            aux= difftime(time_new,time_old)/5;//GUARANTEES IT ADDS 1 REWARD FOR EVERY 5 SECONDS THAT PASSED
            
            while(aux>0){
                for(i=0;i<10;i++){
                    if(rewards[i].flag==0){
                        rewards[i].flag=1;
                        rewards[i].value=RandInt(1,5);
                        do{
                            x=RandInt(1,WINDOW_SIZE-2);
                            y=RandInt(1,WINDOW_SIZE-2);
                        }while(is_free_position(rewards,bots, players, x, y)==false);
                        rewards[i].x=x;
                        rewards[i].y=y;
                        time_old=time_new;
                        break;
                    }
                }
                aux--;
            }
        }
        
        if(check_connection(client_addr, connected_clients)==1){//RESPONDS IF IT IS ONE OF THE CLIENTS
            memcpy( message_to_send.rewards,rewards, 10*sizeof(struct reward));
            sendto(sock_fd, &message_to_send, sizeof(message_s2c), 0, 
                (const struct sockaddr *) &client_addr, client_addr_size);
        }
        delete_and_draw_board(my_win, message_win, players,  bots,  rewards);

    }
    exit(0);
}