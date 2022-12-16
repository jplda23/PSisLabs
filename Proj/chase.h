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

typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

int RandInt(int low, int high) {
    srand(clock());
    return low +  rand()% (high - low + 1);
}

typedef struct message_c2s{
    int type; // -1-connect bot 0-connect human 1-info 2-disconnect
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
    int type; //-1 no more clients allowed, 0 ball info, 1 field status, 2 disconect/dead
    char id;
    int array_pos;
    struct reward rewards[10];
    struct player players[10];
    struct player bots[10];
    
} message_s2c;

bool is_free_position(reward* rewards, player* bots, player* players, int x, int y){
    int i;
    for(i=0;i<10;i++){
        if(rewards[i].x==x){
            if (rewards[i].y==y){
                return false;
            }
        }
        if(players[i].position.x==x){
            if (players[i].position.y==y){
                return false;
            }
        }
        if(bots[i].position.x==x){
            if (bots[i].position.y==y){
                return false;
            }
        }
    }
    return true;
}

void new_player (player_position_t * player, struct player* players, struct player* bots, struct reward* rewards, char c){
    int x,y;
    do{
        x = RandInt(1, WINDOW_SIZE-2);
        y = RandInt(1, WINDOW_SIZE-2);
    }while(is_free_position(rewards, bots, players, x, y)==false);
    player->x=x;
    player->y=y;     
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

void move_player (player_position_t * player, int direction){
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

void draw_board(WINDOW* window, player* previous_players, player* players, player* previous_bots, player* bots, reward* previous_rewards, reward* rewards){
    int i;
    player_position_t dummy_player;
    for (i=0;i<10;i++){
        if(rewards[i].flag==1){//if exists draws
            dummy_player.x=rewards[i].x; //creates a fake player, to reuse the function draw player
            dummy_player.y=rewards[i].y;
            dummy_player.c=rewards[i].value+'0';
            draw_player(window, &dummy_player, true );
        }
        if(bots[i].health==10){//IF is an active bot
            if(previous_bots[i].position.x!=bots[i].position.x || previous_bots[i].position.y!=bots[i].position.y){ //if the position changes
                draw_player(window, &previous_bots[i].position, false);//deletes previous
            }
            draw_player(window, &bots[i].position, true);//draw new
            
        }
        if(players[i].health>0){
            if(previous_players[i].position.x!=players[i].position.x || previous_players[i].position.y!=players[i].position.y){
                draw_player(window, &previous_players[i].position, false);//deletes previous
            }
            draw_player(window, &players[i].position, true);//draw new
        }

    }
}

