#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

int main() {

    int sock;
   
    // 1. Initiate the socket
    // AF_INET is local IP address
    // 0 for TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // struct for the socket address
    struct sockaddr_in server_address;
    // set server address family to IPv4
    server_address.sin_family = AF_INET;

    //specify the port - htons is a utility making sure it is in correct network bite order
    server_address.sin_port = htons(8001);
    // shortcut for localhost IP address
    server_address.sin_addr.s_addr = INADDR_ANY;

    // 2. initiate the connection
    int connection = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));


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