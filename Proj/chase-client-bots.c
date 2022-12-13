#include "chase.h"

WINDOW * message_win;

player_position_t p1;

int main(int argc, char *argv[]){
    
    char* socket_name=argv[argc-1];

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
    message_win = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);


    new_player(&p1, 'y');
    draw_player(my_win, &p1, true);

    int nBots;  // number of bots
    time_t ti, tf;
    int i;
    
    nBots = (int)srand( clock() ) % 10 + 1;

    player bots[nBots];

    direction_t direction;

    time(&ti);
    
    tf = ti + 3000;    

    int key = -1;

    while(key != 27 && key!= 'q'){
        key = wgetch(my_win);

        if (tf - ti > 3000)
        {
            time(&ti);

            for (i = 0; i < nBots; i++)
            {
                
                direction = random()%4;
           
            }
            


        }

        time(&tf);
        

    }

    exit(0);
}