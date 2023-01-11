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
    struct reward rewards[10];
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