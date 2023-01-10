#include "../header/chase.h"

int NPlayers, NBots;

int main(int argc, char *argv[]){

    NBots= atoi(argv[argc-1]);

    //Board variables
    NPlayers=(WINDOW_SIZE-1)*(WINDOW_SIZE-1)-NBots-10;
    player players[NPlayers];
    player bots[NBots];
    reward rewards[10];
    player dummy_player;

    //Board initialization

    init_players_health(players);
    init_bots_health(bots);    
    init_rewards_board(rewards, players, bots);

    // Thread Variables
    pthread_t thread_players[NPlayers];
    pthread_t thread_bots;
    pthread_t thread_prizes;


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


    while(1){


    }
}