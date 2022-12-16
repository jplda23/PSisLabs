#include "chase.h"

WINDOW * message_win;

player_position_t p1;


int main(int argc, char *argv[]){
    
    char* socket_name=argv[argc-1];
    player players[10];
    player bots[10];
    reward rewards[10];

    //socket creation and binding
    int sock_fd;
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd == -1){
	    perror("socket: ");
	    exit(-1);
    } 

    struct sockaddr_un local_client_addr;
    local_client_addr.sun_family = AF_UNIX;
    sprintf(local_client_addr.sun_path,"%s_%d", socket_name, getpid());

    unlink(local_client_addr.sun_path);
    int err = bind(sock_fd, (const struct sockaddr *) &local_client_addr, sizeof(local_client_addr));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, socket_name);

    //SENDING CONNECT MESSAGE AND BEING ATRIBUTED A LETTER
    message_c2s message_to_server;
    message_s2c message_from_server;
    message_to_server.type = 0;//connect type
    sendto(sock_fd, &message_to_server, sizeof(message_c2s), 0, 
            (const struct sockaddr *)&server_addr, sizeof(server_addr));

    //RECEIVE ARRAY_POS AND CHAR FROM SERVER
    recv(sock_fd,&message_from_server, sizeof(message_s2c),0);
    if (message_from_server.type==-1){
        printf("Server is full, please try again soon\n");
        exit(0);
    }
    else if(message_from_server.type==0){
        printf("You are letter %s, Press ENTER to continue\n", &message_from_server.id);
        message_to_server.array_pos=message_from_server.array_pos;
        message_to_server.id=message_from_server.id;
        memcpy(players, message_from_server.players,10*sizeof(struct player));
        memcpy(bots, message_from_server.bots, 10*sizeof(struct player) );
        memcpy(rewards, message_from_server.rewards, 10*sizeof(struct reward));
        getchar();
    }
    else{
        printf("Something went wrong while connecting, please try again soon\n");
        exit(0);
    }

    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* creates a window and draws a border */
    message_win = newwin(10, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);

    draw_board(my_win, players, message_from_server.players, bots, message_from_server.bots, rewards, message_from_server.rewards);

    int i,key = -1;
    while(key != 27 && key!= 'q'){
        key = wgetch(my_win);		
        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
            message_to_server.type=1;
            message_to_server.direction=key;
            sendto(sock_fd, &message_to_server, sizeof(message_c2s), 0, 
            (const struct sockaddr *)&server_addr, sizeof(server_addr));
            recv(sock_fd,&message_from_server, sizeof(message_s2c),0);
            if (message_from_server.type==1){
                draw_board(my_win, players, message_from_server.players, bots, message_from_server.bots, rewards, message_from_server.rewards);
                for(i=0;i<10;i++){
                    if(players[i].health>0){
                        mvwprintw(message_win, i+1,1,"%c %d", players[i].position.c, message_from_server.players[i].health);
                    }
                }
                memcpy(players, message_from_server.players,10*sizeof(struct player));
                memcpy(bots, message_from_server.bots, 10*sizeof(struct player) );
                memcpy(rewards, message_from_server.rewards, 10*sizeof(struct reward));
            }
            if( message_from_server.type==2){
                mvwprintw(message_win, 1,1,"you died");
                break;
            }

        }

        //mvwprintw(message_win, 1,1,"%c key pressed", key);
        wrefresh(message_win);	
    }
    if(key=='q'){
        message_to_server.type=2; //disconnect type
        sendto(sock_fd, &message_to_server, sizeof(message_c2s), 0, 
                    (const struct sockaddr *)&server_addr, sizeof(server_addr));
    }
    printf("Good Game!\n");

    exit(0);
}