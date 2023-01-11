#define WINDOW_SIZE 20
#define FIFO_NAME "/tmp/fifo_chase"
#define NBOTS 10


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
#include <pthread.h>
#include<arpa/inet.h>
#include <stdio.h>



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
    int type; // -1-connect bot | 0-connect human | 1-info | 2 - continue game
    int array_pos; // position of the sender in the player/bot array
    char id;\
    direction_t direction;
} message_c2s;

typedef struct player_t{
    int health;// 0 - player is dead 
    player_position_t position;
} player_t;

typedef struct playerList_t {
    player_t player;
    pthread_t thread_player;
    int client_fd_player;
    struct playerList_t* next;
} playerList_t;

typedef struct reward_t{
    int flag; // 0-not exists or eaten | 1-exists
    int x,y;
    int value;
}reward_t;

/*
    Message Server to Client
*/
typedef struct message_s2c{
    int type; //-1 no more clients allowed | 0 ball info | 2 player_new | 3 player_update | 3 bots info | 4 reward info
    player_t player_dummy;
    reward_t rewards[10];
    player_t bots[10];
    
} message_s2c;

typedef struct thread_args_t{
    int self_client_fd;
    pthread_t self_thread_id;
    playerList_t* list_of_players;
    reward_t* rewards;
    player_t* bots;
} thread_args_t;

int addToListEnd(playerList_t* listInit, playerList_t playerToAdd) {

    playerList_t* aux;
    playerList_t* add;


    if (listInit != NULL){

        for( aux = listInit; aux->next != NULL; aux = aux->next) {
            
        }

        add = (playerList_t*) calloc(1, sizeof(playerList_t));
        
        if (add == NULL)
        {
            printf("Erro na alocação de memória.\n");
            return 0;
        }

        add->next = NULL;
        add->player = playerToAdd.player;
        add->client_fd_player=playerToAdd.client_fd_player;
        add->thread_player=playerToAdd.thread_player;
        aux->next = add;

    return 1;
    }
    return 0;
}

int RemoveFromList(playerList_t *listInit, player_t dummy_player){
    playerList_t *aux, *eliminate;

    for( aux = listInit; aux->next!= NULL; aux = aux->next) {
        if (aux->next->player.position.c==dummy_player.position.c){
            break;
        }
    }
    if(aux->next!=NULL){
        eliminate=aux->next;
        aux->next=aux->next->next;
        free(eliminate);
        return 1;
    }
    else{
        return 0;
    }
    
}


playerList_t* findInList(playerList_t* listInit, char charToFind) {

    playerList_t* aux;

    if (listInit != NULL){

            for( aux = listInit; aux->next != NULL; aux = aux->next) {

                if (aux->next->player.position.c == charToFind) {

                    return aux->next;

                }
            }
    }
    return NULL;    
}



bool is_free_position(reward_t* rewards, player_t* bots, playerList_t* listInit, int x, int y){
    int i;
    playerList_t* aux;

    for(i=0;i<10;i++){
        if(bots[i].position.x==x && bots[i].position.y == y)
            return false;
        if(rewards[i].x==x && rewards[i].y==y)
            return false;
    }

    if (listInit != NULL){

            for( aux = listInit; aux->next != NULL; aux = aux->next) {

                if(aux->next->player.position.x == x && aux->next->player.position.y == y)
                    return false;
            }
    }
    return true;
}

void new_player (player_position_t * player, playerList_t* initList, player_t* bots, reward_t* rewards, char c){
    int x,y;
    do{
        x = RandInt(1, WINDOW_SIZE-2);
        y = RandInt(1, WINDOW_SIZE-2);
    }while(is_free_position(rewards, bots, initList, x, y)==false);
    player->x=x;
    player->y=y;     
    player->c = c;
}

void draw_player(WINDOW *win, player_position_t * player, int deleteVar){
    int ch;
    if(deleteVar){
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

void delete_and_draw_board(WINDOW* window,WINDOW* message_win, playerList_t* listInit, player_t* bots, reward_t* rewards){
    int i,aux=1;
    player_position_t dummy_player;
    playerList_t* auxPlayer;
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
    }

    for( auxPlayer = listInit; auxPlayer->next != NULL; auxPlayer = auxPlayer->next) {
    
        if(auxPlayer->next->player.health > 0 ){
            draw_player(window, &auxPlayer->next->player.position, true);//draw new
            mvwprintw(message_win, aux,1,"%c %d ", auxPlayer->next->player.position.c, auxPlayer->next->player.health);
            aux++;
        }

    }
    wrefresh(window);
    wrefresh(message_win);

}

void init_bots_health(player_t* bot_n){
    int i;
    for(i=0;i<10;i++){
        bot_n[i].health=0;
        bot_n[i].position.c='*';
        bot_n[i].position.x=-1;
        bot_n[i].position.y=-1;
    }
}

void init_rewards_board(reward_t* reward_n, player_t* bots, playerList_t* listInit){
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
        }while(is_free_position(reward_n, bots, listInit, x, y)==false);
        reward_n[i].x=x;
        reward_n[i].y=y;
    }
}

int already_existent_char(playerList_t* listInit, char c){//returns the number of players with that char, expect result 1
    int i, count=0;
    playerList_t* aux;

    for( aux = listInit; aux->next != NULL; aux = aux->next) {
    
        if (aux->next->player.position.c == c)
            count++;
    }
    return count;
}

playerList_t* go_through_player(playerList_t* listInit, player_t* dummy_player){

    int i;

    playerList_t* aux;

    for( aux = listInit; aux->next != NULL; aux = aux->next) {
    
        if (aux->next->player.position.x == dummy_player->position.x && aux->next->player.position.y == dummy_player->position.y)
        {
            return aux->next;
        }  
    }
    return NULL;
}

int go_through_bots(player_t* bots, player_t* dummy_player){

    int i;

    for (i = 0; i < 10; i++)
    {
        if (bots[i].health == 0)
            continue;
        else if (bots[i].position.x == dummy_player->position.x && bots[i].position.y == dummy_player->position.y)
            return i;        
    }

    return -1;
    
}

int go_through_rewards(reward_t* rewards, player_t* dummy_player){

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

int collision_checker(playerList_t* listInit, player_t* dummie_player, player_t* bots, reward_t* rewards, int is_player, int array_position) {

    int i;
    playerList_t* aux, *aux2;

    switch (is_player)
    {
    case 0:     // dummie_player is a bot

        aux = go_through_player(listInit, dummie_player);//Test vs players
        if (aux != NULL) // Found a player in its position
        {
            dummie_player->position.x = bots[array_position].position.x;
            dummie_player->position.y = bots[array_position].position.y;
            aux->player.health = aux->player.health - 1 >= 0 ? aux->player.health - 1 : 0;
            return 1;
        }

        if (go_through_bots(bots, dummie_player) != -1) // Found a bot in its position
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
        
        aux = go_through_player(listInit, dummie_player);
        if (aux != NULL && aux->player.position.c != dummie_player->position.c) // Found a player in its position that is not himself
        {
            // aux is the player that was hit
            // aux2 is the play that hits

            aux2 = findInList(listInit, dummie_player->position.c);

            dummie_player->position.x = aux2->player.position.x;
            dummie_player->position.y = aux2->player.position.y;
            dummie_player->health = dummie_player->health + 1 <= 10 ? dummie_player->health + 1 : 10;
            aux->player.health = aux->player.health - 1 >= 0 ? aux->player.health - 1 : 0;
            return 1;
        }

        if (go_through_bots(bots, dummie_player) != -1) // Found a bot in its position
        {
            aux2 = findInList(listInit, dummie_player->position.c);

            dummie_player->position.x = aux2->player.position.x;
            dummie_player->position.y = aux2->player.position.y;
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

bool check_key(int key){
    if(key==KEY_DOWN || key== KEY_UP || key==KEY_RIGHT || key==KEY_LEFT){
        return true;
    }
    return false;
}

