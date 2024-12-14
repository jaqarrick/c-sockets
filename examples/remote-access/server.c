#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 4444

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    int bytes_read;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_sock);
        exit(1);
    }

    // Listen for connections
    if (listen(server_sock, 5) == -1) {
        perror("Listening failed");
        close(server_sock);
        exit(1);
    }

    printf("Listening on port %d...\n", PORT);

    // Accept a connection
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock == -1) {
        perror("Accept failed");
        close(server_sock);
        exit(1);
    }

    printf("Connection accepted from client\n");

    // Handle communication with the client
    while (1) {
        // Read command from server's stdin
        printf("Enter command: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("fgets failed");
            break;
        }

        // Send command to client
        if (write(client_sock, buffer, strlen(buffer)) == -1) {
            perror("Write failed");
            break;
        }

        // Read response from client
        bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate the buffer
            printf("Client response: %s\n", buffer);
        } else if (bytes_read == 0) {
            printf("Client disconnected\n");
            break;
        } else {
            perror("Read failed");
            break;
        }
    }

    // Clean up
    close(client_sock);
    close(server_sock);

    return 0;
}