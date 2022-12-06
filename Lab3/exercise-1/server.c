
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

int main()
{	
    int fd;

	// TODO_3
    // create and open the FIFO for reading

    while((fd = open(FIFO_LOC, O_RDONLY))== -1){
	  if(mkfifo(FIFO_LOC, 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
	printf("fifo just opened\n");

    // ncurses initialization
	initscr();		    	
	cbreak();				
    keypad(stdscr, TRUE);   
	noecho();			    


    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);

    /* information about the character */
    int ch;
    int pos_x;
    int pos_y;
    int msg_size;


    
    direction_t  direction;

    while (1)
    {
        // TODO_7
        // receive message from the clients
        /*if ((read(fd,&msg_size,sizeof(int))) < 0){
            perror("error msg");
        }*/
        msg_size=sizeof(remote_char_t);
        struct remote_char_t *msg=malloc(msg_size);
        if ((read(fd, msg, msg_size))<0){
            perror("error msg");
        }
        
        //TODO_8
        // process connection messages
        if (msg->msg_type==0){
            ch = msg->ch;
            pos_x=WINDOW_SIZE/2;
            pos_y=WINDOW_SIZE/2;
        }

        // TODO_11
        // process the movement message
        else if (msg->msg_type==1){
            wmove(my_win, pos_x, pos_y);
            waddch(my_win,' ');
            wrefresh(my_win);
            switch (msg->direction)
            {
            case LEFT:
                pos_y=MAX(pos_y-1,0);
                break;
            case RIGHT:
                pos_y=MIN(pos_y+1,WINDOW_SIZE);
                break;
            case DOWN:
                pos_x=MIN(pos_x+1,WINDOW_SIZE);
                break;
            case UP:
                pos_x=MAX(pos_x-1,0);
                break;
            default:
                    break;
        }
        }
        
        free(msg);
        /* draw mark on new position */
        wmove(my_win, pos_x, pos_y);
        waddch(my_win,ch| A_BOLD);
        wrefresh(my_win);			
    }
  	endwin();			/* End curses mode		  */

	return 0;
}