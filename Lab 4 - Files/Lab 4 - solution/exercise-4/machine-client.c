#include "remote-char.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>

int main()
{	

    //TODO_4
    // create and open the FIFO for writing

    int sock_fd;
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd == -1){
	    perror("socket: ");
	    exit(-1);
    }  

    struct sockaddr_un local_client_addr;
    local_client_addr.sun_family = AF_UNIX;
    sprintf(local_client_addr.sun_path,"%s_%d", SOCKET_NAME, getpid());

    unlink(local_client_addr.sun_path);
    int err = bind(sock_fd, 
                (const struct sockaddr *)&local_client_addr, sizeof(local_client_addr));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }


    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_NAME);

    //TODO_5
    // read the character from the user
    char ch;
    char linha[100];
    do{
        printf("what is your symbol(a..z)?: ");
        ch = getchar();
        ch = tolower(ch);  
    }while(!isalpha(ch));

    // TODO_6
    // send connection message
    remote_char_t m, m_receive;
    m.msg_type = 0;
    m.ch = ch;
    sendto(sock_fd, &m, sizeof(remote_char_t), 0, 
        (const struct sockaddr *) &server_addr, sizeof(server_addr));
    
    

    int sleep_delay;
    direction_t direction;
    int n = 0;
    while (1)
    {
        n++;
        sleep_delay = random()%700000;
        usleep(sleep_delay);
        direction = random()%4;
        switch (direction)
        {
        case LEFT:
           printf("%d Going Left   \n", n);
            break;
        case RIGHT:
            printf("%d Going Right   \n", n);
           break;
        case DOWN:
            printf("%d Going Down   \n", n);
            break;
        case UP:
            printf("%d Going Up    \n", n);
            break;
        }

        //TODO_9
        // prepare the movement message
        m.direction = direction;
        m.msg_type = 1;

        //TODO_10
        //send the movement message
        sendto(sock_fd, &m, sizeof(remote_char_t), 0, 
            (const struct sockaddr *) &server_addr, sizeof(server_addr));
        recv(sock_fd, &m_receive, sizeof(remote_char_t), 0);
        if(m_receive.msg_type==3){
            printf("in the same position\n");
        }
    }

 
	return 0;
}