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
    int type; // -1-connect bot | 0-connect human | 1-info | 2 - continue game
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
    int type; //-1 no more clients allowed | 0 ball info | 1 field status
    char id;
    int array_pos;
    struct reward rewards[10];
    struct player players[10];
    struct player bots[10];
    
} message_s2c;
