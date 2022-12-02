#include "remote-char.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int main()
{	

    int fd;

    //TODO_4
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

    char str[100];
    int msg_size;
    printf("What letter will you be?\n");
    fgets(str, 100, stdin);
    str[strlen(str)-1] = '\0';
    // TODO_6
    msg_size= sizeof(remote_char_t);
    struct remote_char_t msg ;
    msg.msg_type = 0;
    msg.ch = *str;

    /*write(fd,&msg_size,sizeof(int));*/
    write(fd, &msg, msg_size);
    

    int sleep_delay;
    direction_t direction;
    int n = 0;
    msg.msg_type=1;
    while (1)
    {
        sleep_delay = random()%50000;
        usleep(sleep_delay);
        direction = random()%4;
        n++;
        
        /*switch (direction)
        {
        case LEFT:
           printf("%d Going Left   ", n);
            break;
        case RIGHT:
            printf("%d Going Right   ", n);
           break;
        case DOWN:
            printf("%d Going Down   ", n);
            break;
        case UP:
            printf("%d Going Up    ", n);
            break;
        }*/
         //TODO_9
        // prepare the movement message
        
        msg.direction = direction;
        
        

        //TODO_10
        //send the movement message

        /*write(fd,&msg_size,sizeof(int));*/
        int n;
        n=write(fd, &msg, msg_size);
        if (n<=0){
            printf("error");
        }
        else{
            printf("ola");
        }
    }

    
	return 0;
}