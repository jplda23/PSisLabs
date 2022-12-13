
typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct remote_char_t
{   
    int msg_type; /* 0 join   1 - move  2 - reply no eat 4 - reply eat*/
    char ch; 
    direction_t direction ;
    /* data */
}remote_char_t;

#define SOCKET_NAME "/tmp/sock_snail"
