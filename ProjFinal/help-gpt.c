#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

// Structure for passing arguments to the thread function
typedef struct {
    int client_fd;
} thread_args;

// Thread function for receiving data from a client
void *receive_data(void *arg) {
    thread_args *args = (thread_args *)arg;
    int client_fd = args->client_fd;

    // Receive data from the client
    char buffer[1024];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error receiving data from client");
        exit(EXIT_FAILURE);
    }

    // Print the received data
    printf("Received %d bytes: %s\n", bytes_received, buffer);

    // Close the connection
    close(client_fd);
    free(arg);

    return NULL;
}

int main(int argc, char *argv[])
{
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the client address and port
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr("client_address");
    client_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Error binding socket to client address");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    listen(sockfd, 5);

    while (1) {
        // Accept an incoming connection
        int client_fd = accept(sockfd, NULL, NULL);
        if (client_fd < 0) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        // Create a new thread to receive data from the client
        pthread_t thread;
        thread_args *args = malloc(sizeof(thread_args));
        args->client_fd = client_fd;
        int rc = pthread_create(&thread, NULL, receive_data, (void *)args);
        if (rc != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
