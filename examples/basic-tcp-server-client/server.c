#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#define PORT 8080

int main(){

    char server_message[256] = "Hello from the server!";

    int server_sock;
    // 1. Initiate the server socket, i.e. create a new connection endpoint
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    printf("server socket file descriptor: %d\n", server_sock);
    char hello[256] = "Hello to stdout!";
    if(server_sock == -1){
        printf("Could not create server socket\n");
    }
    // define a struct for the server address
    struct sockaddr_in server_address;
    // using IP
    server_address.sin_family = AF_INET;
    // set address port
    server_address.sin_port = htons(PORT);
    // set local IP address (shortcut)
    server_address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

    // 2. Bind the socket to the address, i.e. attach a local address to a socket
    bind(server_sock, (struct sockaddr*) &server_address, sizeof(server_address));

    printf("Binding socket to port\n\n");
    // 3. Listen for connections (up to 5 connections)
    listen(server_sock, 5);

    printf("Listening for connections\n");
    // 4. Accept connections
    int client_socket;
    // Since address is local, we don't need last two args. 
    client_socket = accept(server_sock, NULL, NULL);

    printf("Sending a message to the client!\n");
    // once connected, send the message
    send(client_socket, server_message, sizeof(server_message), 0);

    int status = close(server_sock);

    if(status == -1){
        printf("Error closing the socket connection.\n");
        return 0;
    }

    printf("Closing the socket connection!\n");

    return 0;

}