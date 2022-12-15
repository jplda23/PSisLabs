#include "chase.h"

WINDOW * message_win;

int RandInt(int low, int high) {
    srand(clock());
    return low +  rand()% (high - low + 1);
}

void init_players_health(player* player_n){
    int i;
    for(i=0;i<10;i++){
        player_n[i].health=0;
        player_n[i].position.c='1';
        player_n[i].position.x=-1;
        player_n[i].position.y=-1;
    }
}

void init_bots_health(player* bot_n){
    int i;
    for(i=0;i<10;i++){
        bot_n[i].health=0;
        bot_n[i].position.c='*';
        bot_n[i].position.x=-1;
        bot_n[i].position.y=-1;
    }
}

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



int already_existent_char(player* player_n, char c){//returns the number of players with that char, expect result 1
    int i, count=0;
    for(i=0;i<10;i++){
        if(player_n[i].position.c==c){
            count++;
        }
    }
    return count;
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

int check_cheating(player players[], player player_check,struct sockaddr_un client_addr, struct sockaddr_un connected_clients[], int array_pos ){//TO BE BETTER DEVELOPED
    int i,j;
    for(i=0;i<10;i++){
        if(players[i].position.c==player_check.position.c){
            break;
        }
    }
    for(j=0;j<10;j++){
        if(connected_clients[j].sun_path==client_addr.sun_path){
            break;
        }
    }
    if(i==j){
        if(j==array_pos){
            return array_pos;
        }
        else{
            return j;
        }
    }
    else{//maybe this can be just return j since I dont think you can change yout socket name
        return -1;
    }
}

bool check_connection(struct sockaddr_un client_addr, struct sockaddr_un connected_clients[]){
    int i;
    for(i=0;i<10;i++){
        if(strcmp(client_addr.sun_path,connected_clients[i].sun_path)==0){
            return true;
        }
    }
    return false;
}

bool collision_checker(player* dummie_player, int type) {

    int i;

    switch (type)
    {
    case 0:     // bot
        
        for (i = 0; i < 10; i++)
        {
            /* code */
        }
        
        break;

    case 1:    // Player
        /* code */
        break;    
    
    default:
        break;
    }

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
    player bots[10];
    player dummy_player;
    init_players_health(players);
    init_bots_health(bots);    
    reward rewards[10];
    init_rewards_board(rewards, players, bots);
    int n_bytes;
    message_s2c message_to_send;
    message_c2s message_received;
    int array_pos,x, y;
    int i;


    struct sockaddr_un connected_clients[11];//To save all the connected clients and not read from unconnected
    struct sockaddr_un client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_un);
    
    time_t time_new, time_old;
    time(&time_old);

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
            do{
                new_player(&players[array_pos].position,RandInt('a','Z')); 
            }while(already_existent_char(players,players[array_pos].position.c)!=1);
            connected_clients[array_pos]=client_addr;
            message_to_send.type=0;
            message_to_send.array_pos=array_pos;
            message_to_send.id=players[array_pos].position.c;
            sendto(sock_fd, &message_to_send, sizeof(message_s2c), 0, 
                    (const struct sockaddr *) &client_addr, client_addr_size);
            
        }
        
        else if (message_received.type == -1) {
            // connect from bot - initialize
            connected_clients[10]=client_addr;
            bots[message_received.array_pos].health = 10;
            do{
                x = RandInt(1, WINDOW_SIZE-2);
                y = RandInt(1, WINDOW_SIZE-2);
            }while(is_free_position(rewards, bots, players, x, y)==false);
            
            bots[message_received.array_pos].position.x = x;
            bots[message_received.array_pos].position.y = y;
                          
        }
        else if(message_received.type == 1){
            if(check_connection(client_addr, connected_clients)==true){ //IS HUMAN AND WILL UPDATE THE HUMAN, WILL HAVE TO INTRODUCE A CHEATER CHECK TOO
                array_pos=message_received.array_pos;
                dummy_player.position.x=players[array_pos].position.x;
                dummy_player.position.y=players[array_pos].position.y;
                dummy_player.position.c=players[array_pos].position.c;
                dummy_player.health=players[array_pos].health;
                move_player(dummy_player, message_received.direction);
                //Check for collision and if collision
                if(dummy_player.health==0){
                    message_to_send.type=2;
                    message_to_send.players=players;
                    message_to_send.bots=bots;
                    message_to_send.rewards=rewards;
                    message_to_send.array_pos=array_pos;
                    message_to_send.id=dummy_player.c;
                    sendto(sock_fd, &message_to_send, sizeof(message_s2c), 0, 
                        (const struct sockaddr *) &client_addr, client_addr_size); //SEND MESSAGE BEFORE UPDATING THE PLAYERS SO THE CLIENT CAN DRAW DEAD BOARD
                    players[array_pos].health=0;
                    players[array_pos].position.c='1';
                    players[array_pos].position.x=-1;
                    players[array_pos].position.y=-1;
                }
                else if(dummy_player.health>0){
                    players[array_pos]=dummy_player;
                    message_to_send.type=1;
                    message_to_send.players=players;
                    message_to_send.bots=bots;
                    message_to_send.rewards=rewards;
                    message_to_send.array_pos=array_pos;
                    message_to_send.id=dummy_player.c;
                    sendto(sock_fd, &message_to_send, sizeof(message_s2c), 0, 
                        (const struct sockaddr *) &client_addr, client_addr_size);

                }
            }
            else if(client_addr.sun_path==connected_clients[10].sun_path){ //IS A BOT AND WILL UPDATE THE BOT
                array_pos=message_received.array_pos;
                dummy_player.position.x=bots[array_pos].position.x;
                dummy_player.position.y=bots[array_pos].position.y;
                dummy_player.position.c=bots[array_pos].position.c;
                dummy_player.health=10;
                move_player(dummy_player, message_received.direction);
                //Check for collision and if collision 
                players[array_pos]=dummy_player;
                message_to_send.type=1;
                message_to_send.players=players;
                message_to_send.bots=bots;
                message_to_send.rewards=rewards;
                message_to_send.array_pos=array_pos;
                message_to_send.id=dummy_player.c;
                sendto(sock_fd, &message_to_send, sizeof(message_s2c), 0, 
                    (const struct sockaddr *) &client_addr, client_addr_size);

            }
            
        }

        else if (message_received.type == 2){
            if (check_connection(client_addr, connected_clients)==true){
                //DISCONNECT THE CLIENT FROM THE SERVER
                array_pos=message_received.array_pos;
                strcpy(connected_clients[array_pos].sun_path,socket_name);//maybe change this, I just want to reset the memory
                players[array_pos].position.c='1';
                players[array_pos].health=0;
                players[array_pos].position.x=-1;
                players[array_pos].position.y=-1;
            }

            
        }

        time(&time_new);
        if (difftime(time_new,time_old)>=5)//EVERY 5 SECONDS ADDS 1 REWARD(if less than 10)
        {
            time_old=time_new;
            for(i=0;i<10;i++){
                if(rewards[i].flag==0){
                    rewards[i].flag=1;
                    rewards[i].value=RandInt(1,5);
                    do{
                        x=RandInt(1,WINDOW_SIZE-2);
                        y=RandInt(1,WINDOW_SIZE-2);
                    }while(is_free_position(rewards,bots, players, x, y)==false);
                    rewards[i].x=x;
                    rewards[i].y=y;
                    printf("newreward\n");
                    break;
                }
            }
        }
        

    }
    exit(0);

}