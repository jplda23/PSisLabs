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

/*
    Generates random int numbers bounded by a min and a max number.
    Uses clock() as a seed.
*/
int RandInt(int low, int high) {
    srand(clock());
    return low +  rand()% (high - low + 1);
}

/*
    Message Client to Server
*/
typedef struct message_c2s{
    int type; // -1-connect bot | 0-connect human | 1-info | 2-disconnect
    int array_pos; // position of the sender in the player/bot array
    char id;
    direction_t direction;
} message_c2s;

typedef struct player{
    int health;// 0 - player is dead
    player_position_t position;
} player;

typedef struct reward{
    int flag; // 0-not exists or eaten | 1-exists
    int x,y;
    int value;
}reward;

/*
    Message Server to Client
*/
typedef struct message_s2c{
    int type; //-1 no more clients allowed | 0 ball info | 1 field status | 2 disconect/dead
    char id;
    int array_pos;
    struct reward rewards[10];
    struct player players[10];
    struct player bots[10];
    
} message_s2c;

/*
    Checks if the position x,y is free: without players, bots or rewards
*/
bool is_free_position(reward* rewards, player* bots, player* players, int x, int y){
    int i;
    for(i=0;i<10;i++){
        if(rewards[i].x==x && rewards[i].y==y)
            return false;
        if(players[i].position.x==x && players[i].position.y==y)
            return false;
        if(bots[i].position.x==x){
            if (bots[i].position.y==y){
                return false;
            }
        }
    }
    return true;
}

/*
    Initializes the new player with a random free position and the given symbol
*/
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

/*
    Uses ncurses functions to draw player on the terminal
*/
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

/*
    Uses ncurses functions to move player on the terminal
*/
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

/*
    Uses ncurses functions to draw the new board.
*/
void delete_and_draw_board(WINDOW* window,WINDOW* message_win, player* players, player* bots, reward* rewards){
    int i,aux=1;
    player_position_t dummy_player;
    werase(window);
    werase(message_win);
    box(window, 0 , 0);	
    box(message_win, 0 , 0);

    for(i=0;i<10;i++){
        if(rewards[i].flag==1){//if exists draws
            dummy_player.x=rewards[i].x; //creates a player variable, to draw a reward, reuse«ing the function draw player
            dummy_player.y=rewards[i].y;
            dummy_player.c=rewards[i].value+'0';
            draw_player(window, &dummy_player, true );
        }
        if(bots[i].health==10){//IF is an active bot
            draw_player(window, &bots[i].position, true);//draw new
            
        }
        if(players[i].health>0){
            draw_player(window, &players[i].position, true);//draw new
            mvwprintw(message_win, aux,1,"%c %d ", players[i].position.c, players[i].health);
            aux++;
        }

    }
    wrefresh(window);
    wrefresh(message_win);

}

/*
    Initializes player's health to 0, since it is assumed that if
    the player has health == 0, then it doesn't exist.
    Position is initialized outside of the board.
*/
void init_players_health(player* player_n){
    int i;
    for(i=0;i<10;i++){
        player_n[i].health=0;
        player_n[i].position.c='1';
        player_n[i].position.x=-1;
        player_n[i].position.y=-1;
    }
}

/*
    Bots' health is initialized to 0. It is assumed that if the health == 0,
    then it doesn't exist.
    Position is initialized outside of the board. Char is initialized to *.
*/
void init_bots_health(player* bot_n){
    int i;
    for(i=0;i<10;i++){
        bot_n[i].health=0;
        bot_n[i].position.c='*';
        bot_n[i].position.x=-1;
        bot_n[i].position.y=-1;
    }
}

/*
    Rewards are initialized.
    If a reward has its value to 0, then it doesn't exist or was just eaten.
    Value is initialized randomly between 1 and 5.
    Position is also random and checked if its free.
*/
void init_rewards_board(reward* reward_n, player* bots, player* players){
    int i,x,y;
    for(i=0;i<10;i++){
        reward_n[i].flag=0;
        reward_n[i].x=-1;
        reward_n[i].y=-1;
    }
    for(i=0;i<5;i++){
        reward_n[i].value=RandInt(1,5);
        reward_n[i].flag=1;
        do{
        x=RandInt(1,WINDOW_SIZE-2);
        y=RandInt(1,WINDOW_SIZE-2);
        }while(is_free_position(reward_n, bots, players, x, y)==false);
        reward_n[i].x=x;
        reward_n[i].y=y;
    }
}

/*
    Checks if the char for that player is already in use.
*/
int already_existent_char(player* player_n, char c){//returns the number of players with that char, expect result 1
    int i, count=0;
    for(i=0;i<10;i++){
        if(player_n[i].position.c==c){
            count++;
        }
    }
    return count;
}

/*
    Checks if there's still a slot available for the player, out of the existant 10.
    If there are less than 10 players, the first free position is returned.
    Else, it returns -1 (no free positions).
*/
int get_player_input_array_position(player player_n[], int max_size){
    int i;
    for (i=0; i<max_size; i++){
        if (player_n[i].health==0){
            return i;
        }
    }
    return -1;
}

/*
    Check for possible malitious intent in the messages received such as providing a wrong array position
    which would result in affecting a different player/ the robots or segmentation fault
    returns true if the player is "cheating"
*/
bool check_cheating(char saved, char provided ,struct sockaddr_un client_addr, struct sockaddr_un connected_clients[], int array_pos ){
    if(array_pos>=10 || array_pos<0){
        return true;
    }
    if(saved!=provided){
        return true;
    }
    if(strcmp(connected_clients[array_pos].sun_path,client_addr.sun_path)!=0){
        return true; 
    }
    return false;
}

/*
    Checks if the message comes from a connected client, which was previously saved when sending a connection message
    This makes the program ignore messages from unconnected clients
    returns true if connected, false if not
*/
bool check_connection(struct sockaddr_un client_addr, struct sockaddr_un connected_clients[]){
    int i;
    for(i=0;i<10;i++){
        if(strcmp(client_addr.sun_path,connected_clients[i].sun_path)==0){
            return true;
        }
    }
    return false;
}
/*
    Goes through the struct player array and checks if there's any player
    with the same position.
    Returns the array position of the player in that position or -1 if empty.
*/
int go_through_player(player* players, player* dummy_player){

    int i;

    for (i = 0; i < 10; i++)
    {
        if (players[i].health == 0)
            continue;
        else if (players[i].position.x == dummy_player->position.x && players[i].position.y == dummy_player->position.y)
            return i;        
    }

    return -1;
    
}

/*
    Goes through the struct reward array and checks if there's any reward
    with the same position.
    Returns the array position of the reward in that position or -1 if empty.
*/
int go_through_rewards(reward* rewards, player* dummy_player){

    int i;

    for (i = 0; i < 10; i++)
    {
        if (rewards[i].flag == 0)
            continue;
        else if (rewards[i].x == dummy_player->position.x && rewards[i].y == dummy_player->position.y)
            return i;        
    }

    return -1;
    
}

/*
    Implements the collision checker according to the rules in the statement.
    Since what happens in case it's a bot or a player is different, there's a switch
    stamentent for that.
    Then it checks if there is any player, bot or reward in that position and acts accordingly.
*/
int collision_checker(player* players, player* dummie_player, player* bots, reward* rewards, int is_player, int array_position) {

    int i;

    switch (is_player)
    {
    case 0:     // dummie_player is a bot

        i = go_through_player(players, dummie_player);//Test vs players
        if (i != -1) // Found a player in its position
        {
            dummie_player->position.x = bots[array_position].position.x;
            dummie_player->position.y = bots[array_position].position.y;
            players[i].health = players[i].health - 1 >= 0 ? players[i].health - 1 : 0;
            return 1;
        }

        if (go_through_player(bots, dummie_player) != -1) // Found a bot in its position
        {
            dummie_player->position.x = bots[array_position].position.x;
            dummie_player->position.y = bots[array_position].position.y;
            return 1;
        }

        if (go_through_rewards(rewards, dummie_player) != -1) // Found a prize
        {
            dummie_player->position.x = bots[array_position].position.x;
            dummie_player->position.y = bots[array_position].position.y;
            return 2;        
        }       
        
        break;

    case 1:    // dummie_player is a Player
        
        i = go_through_player(players, dummie_player);
        if (i != -1 && i != array_position) // Found a player in its position that is not himself
        {
            dummie_player->position.x = players[array_position].position.x;
            dummie_player->position.y = players[array_position].position.y;
            dummie_player->health = dummie_player->health + 1 <= 10 ? dummie_player->health + 1 : 10;
            players[i].health = players[i].health - 1 >= 0 ? players[i].health - 1 : 0;
            return 1;
        }

        if (go_through_player(bots, dummie_player) != -1) // Found a bot in its position
        {
            dummie_player->position.x = players[array_position].position.x;
            dummie_player->position.y = players[array_position].position.y;
            return 1;
        }

        i = go_through_rewards(rewards, dummie_player);
        if (i != -1) // Found a prize
        {
            rewards[i].flag = 0;
            dummie_player->health = dummie_player->health + rewards[i].value <= 10 ? dummie_player->health + rewards[i].value  : 10;
            return 2;
        } 

        break;    
    
    default:
        break;
    }
    return 0;

}

/*
    Checks if the key pressed by the human client was valid
*/
bool check_key(int key){
    if(key==KEY_DOWN || key== KEY_UP || key==KEY_RIGHT || key==KEY_LEFT){
        return true;
    }
    return false;
}




