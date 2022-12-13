#define WINDOW_SIZE 20
#define FIFO_NAME "/tmp/fifo_chase"


#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

typedef struct player_position_t{
    int x, y;
    char c;
} player_position_t;

typedef struct message_c2s{
    int type; // 0-connect 1-info 2-disconnect
    player_position_t player_info;
} message_c2s;

typedef struct player_list{
    player_position_t player;
    struct player_list* next;
} player_list;

typedef struct reward{
    int x,y;
    int value;
    struct reward* next;
}reward;

typedef struct message_s2c{
    reward* reward_list_pointer;
    player_list* bots;
    player_list* players;
    
} message_s2c;


void new_player (player_position_t * player, char c){
    player->x = WINDOW_SIZE/2;
    player->y = WINDOW_SIZE/2;
    player->c = c;
}

void draw_player(WINDOW *win, player_position_t * player, int delete){
    int ch;
    if(delete){
        ch = player->c;
    }else{
        ch = ' ';
    }
    int p_x = player->x;
    int p_y = player->y;
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
}

void moove_player (player_position_t * player, int direction){
    if (direction == KEY_UP){
        if (player->y  != 1){
            player->y --;
        }
    }
    if (direction == KEY_DOWN){
        if (player->y  != WINDOW_SIZE-2){
            player->y ++;
        }
    }
    

    if (direction == KEY_LEFT){
        if (player->x  != 1){
            player->x --;
        }
    }
    if (direction == KEY_RIGHT)
        if (player->x  != WINDOW_SIZE-2){
            player->x ++;
    }
}
