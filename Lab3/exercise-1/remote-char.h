// TODO_1 
// declaration the struct corresponding to the exchanged messages
typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct remote_char_t{
    int msg_type;           // 0 - join/connect | 1 - move 
    char ch;                // user client defined char
    direction_t direction;  // UP, DOWN, LEFT, RIGHT
} remote_char_t;

// TODO_2
//declaration of the FIFO location

#define FIFO_LOC  "/tmp/fifo_server"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))