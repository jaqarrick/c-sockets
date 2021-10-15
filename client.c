#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#define PORT 8080

int main(int argc, char* argv[]) {

    int sock;
   
    // 1. Initiate the socket
    // AF_INET is local IP address
    // 0 for TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1){
        printf("Count not create client socket");
    }
    // struct for the socket address
    struct sockaddr_in server_address;
    // set server address family to IPv4
    server_address.sin_family = AF_INET;

    //specify the port - htons is a utility making sure it is in correct network bite order
    server_address.sin_port = htons(PORT);

    if(argc < 2){
        printf("No IP address declared, defaulting to localhost.\n");
        // shortcut for local IP addr
        server_address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    } else {
        printf("Attempting to connect to %s\n", argv[1]);
        server_address.sin_addr.s_addr = inet_addr(argv[1]);
    }
    // 2. initiate the connection
    int connection = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));

    // error handling
    if(connection == -1){
        printf("There is an issue with the connection.\n");
        // early return
        return 0;
    }

    // store the server response
    char server_response[256];
    recv(sock, &server_response, sizeof(server_response), 0);
    printf("The server response is %s\n", server_response);

    // close the connection
    close(sock);

    return 0;
}