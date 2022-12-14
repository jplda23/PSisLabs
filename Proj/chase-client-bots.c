#include "chase.h"

int main(int argc, char *argv[]){
    
    char* socket_name=argv[argc-2];

    // ATENÇÃO
    // ATENÇÃO
    // ATENÇÃO
    // ATENÇÃO
    // CÓDIGO DOS SOCKETS POR REVER!!!!!!

    //socket creation and binding
    int sock_fd;
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd == -1){
	    perror("socket: ");
	    exit(-1);
    }  

    struct sockaddr_un local_client_addr;
    local_client_addr.sun_family = AF_UNIX;
    sprintf(local_client_addr.sun_path,"%s_%d", socket_name, getpid());

    unlink(local_client_addr.sun_path);
    int err = bind(sock_fd, (const struct sockaddr *) &local_client_addr, sizeof(local_client_addr));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }


    int nBots;  // number of bots
    int i;

    nBots = argv[argc-1];
    message_c2s m2s;

    m2s.type = -1;
    m2s.id = '*';

    for (i = 0; i < nBots; i++)
    {
        m2s.array_pos = i;

        // Está incompleto, mas fica a intenção
        sendto(sock_fd, &m2s, sizeof(message_c2s), 0, 
            (const struct sockaddr *) &server_addr, sizeof(server_addr));
    }

    m2s.type = 1;
    m2s.id = '*';
    
    while(1){

        sleep((unsigned) 3);

        for (i = 0; i < nBots; i++)
        {

            m2s.array_pos = i;
            m2s.direction = random()%4;

            // Está incompleto, mas fica a intenção
            sendto(sock_fd, &m2s, sizeof(message_c2s), 0, 
                (const struct sockaddr *) &server_addr, sizeof(server_addr));                
        
        }
    }

    exit(0);
}