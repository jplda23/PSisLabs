
#include <ncurses.h>
#include "remote-char.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <stdlib.h>
#define WINDOW_SIZE 15

direction_t random_direction(){
    return  random()%4;
}
void new_position(int* x, int *y, direction_t direction){
    switch (direction)
    {
    case UP:
        (*x) --;
        if(*x ==0)
            *x = 2;
        break;
    case DOWN:
        (*x) ++;
        if(*x ==WINDOW_SIZE-1)
            *x = WINDOW_SIZE-3;
        break;
    case LEFT:
        (*y) --;
        if(*y ==0)
            *y = 2;
        break;
    case RIGHT:
        (*y) ++;
        if(*y ==WINDOW_SIZE-1)
            *y = WINDOW_SIZE-3;
        break;
    default:
        break;
    }
}

int main(){	



	// TODO_3
    // create and open the FIFO for reading
    int fd;
	while((fd = open(FIFO_NAME, O_RDONLY))== -1){
	  if(mkfifo(FIFO_NAME, 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
	printf("fifo just opened\n");

	initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);

    /* information about the character */
    int ch;
    int pos_x;
    int pos_y;


    direction_t  direction;


    // TODO_7
    // receive message from the clients
    remote_char_t m;

    while (1)
    {

        // TODO_7
        // receive message from the clients
        read(fd, &m, sizeof(remote_char_t));

        //TODO_8
        // process connection messages
        if(m.msg_type == 0){
            ch = m.ch;
            pos_x = WINDOW_SIZE/2;
            pos_y = WINDOW_SIZE/2;
        }


        // TODO_11
        // process the movement message
        if(m.msg_type == 1){

            /*deletes old place */
            wmove(my_win, pos_x, pos_y);
            waddch(my_win,' ');

            /* claculates new direction */
            direction = m.direction;

            /* claculates new mark position */
            new_position(&pos_x, &pos_y, direction);
        }
        
        /* draw mark on new position */
        wmove(my_win, pos_x, pos_y);
        waddch(my_win,ch| A_BOLD);
        wrefresh(my_win);			
    }
  	endwin();			/* End curses mode		  */

	return 0;
} 