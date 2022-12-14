#include "chase.h"

WINDOW * message_win;

int RandInt(int low, int high) {
  return low + rand() % (high - low + 1);
}

void init_players_health(player* player_n){
    int i;
    for(i=0;i<10;i++){
        player_n[i].health=0;
        player_n[i].position.c='1';
    }
}

int get_player_input_array_position(player player_n[], int max_size){
    int i;
    for (i=0; i<max_size; i++){
        if (player_n[i].health==0){
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[]){

    char* socket_name=argv[argc-1];
    
    //Socket creation and binding
    int sock_fd;
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd == -1){
	    perror("socket: ");
	    exit(-1);
    }
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path, socket_name);

    unlink(socket_name);
    int err = bind(sock_fd, 
            (const struct sockaddr *)&local_addr, sizeof(local_addr));
    if(err == -1) {
	    perror("bind");
	    exit(-1);
    }

    player players[10];
    init_players_health(players);
    player bots[10];
    reward rewards[10];
    int n_bytes;
    message_s2c message_to_send;
    message_c2s message_received;
    int array_pos;

    struct sockaddr_un client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_un);

    while(1){
        n_bytes = recvfrom(sock_fd, &message_received, sizeof(message_c2s), 0, 
                        (const struct sockaddr *)&client_addr, &client_addr_size);
        if (n_bytes!= sizeof(message_c2s)){
            continue;
        }
        if (message_received.type==0){
            //initiate player in array
            array_pos=get_player_input_array_position(players, 10);
            players[array_pos].health=10;
            players[array_pos].position.x=WINDOW_SIZE/2;
            players[array_pos].position.y=WINDOW_SIZE/2;
            players[array_pos].position.c=RandInt('a','Z');

            message_to_send.type=0;
            message_to_send.array_pos=array_pos;
            message_to_send.id=players[array_pos].position.c;
            printf("%d",array_pos);
            sendto(sock_fd, &message_to_send, sizeof(message_s2c), 0, 
                    (const struct sockaddr *) &client_addr, client_addr_size);
            
        }

    }
    exit(0);

}