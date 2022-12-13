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

typedef struct player_position_t{
    int x, y;
    char c;
} player_position_t;

typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct message_c2s{
    int type; // 0-connect 1-info 2-disconnect
    int array_pos;
    char id;
    direction_t direction;
} message_c2s;

typedef struct player{
    int health;// 0-jogador dead
    player_position_t position;
} player;

typedef struct reward{
    int flag; // 0-not exists or eaten, 1-exists
    int x,y;
    int value;
}reward;

typedef struct message_s2c{
    int type; //0 ball info, 1 field status, 2 disconect/dead
    char id;
    struct reward rewards[10];
    struct player players[10];
    struct player bots[10];
    
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
