#include <ncurses.h>
#include "remote-char.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>
 

int main()
{

    int fd;

   
    //TODO_4
    // create and open the FIFO for writing

    while((fd = open(FIFO_LOC, O_WRONLY))== -1){
	  if(mkfifo(FIFO_LOC, 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
	printf("fifo just opened\n");


    //TODO_5
    // read the character from the user

    char str[100];
    int msg_size;
    printf("What letter will you be?\n");
    fgets(str, 100, stdin);

    str[strlen(str)-1] = '\0';

    // TODO_6
    // send connection message
    msg_size= sizeof(remote_char_t);
    struct remote_char_t* msg = malloc(msg_size);
    msg->msg_type = 0;
    msg->ch = *str;

    write(fd,&msg_size,sizeof(int));
    write(fd, msg, msg_size);




	initscr();			/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

    
    int ch;

    msg->msg_type = 1;
    int n = 0;
    do
    {
    	ch = getch();		
        n++;
        switch (ch)
        {
            case KEY_LEFT:
                mvprintw(0,0,"%d Left arrow is pressed", n);
                break;
            case KEY_RIGHT:
                mvprintw(0,0,"%d Right arrow is pressed", n);
                break;
            case KEY_DOWN:
                mvprintw(0,0,"%d Down arrow is pressed", n);
                break;
            case KEY_UP:
                mvprintw(0,0,"%d :Up arrow is pressed", n);
                break;
            default:
                ch = 'x';
                    break;
        }
        refresh();			/* Print it on to the real screen */
        //TODO_9
        // prepare the movement message
        
        msg->direction = ch;
        
        

        //TODO_10
        //send the movement message

        write(fd,&msg_size,sizeof(int));
        write(fd, msg, msg_size);
        
    }while(ch != 27);
    
    free(msg);
    
  	endwin();			/* End curses mode		  */

	return 0;
}