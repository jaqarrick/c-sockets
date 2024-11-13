#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
    const char *ip;
    int start_port;
    int end_port;
} ScannerArgs;

void validate_port(int port)
{
    if (port < 1 || port > 65535)
    {
        printf("Invalid port number: %d. Port number must be between 1 and 65535.\n", port);
        exit(1);
    }
}

void validate_args(int argc, char *argv[], ScannerArgs *scanner_args)
{
    if (argc < 4)
    {
        printf("Usage: %s <IP> <start_port> <end_port>\n", argv[0]);
        exit(1);
    }

    scanner_args->ip = argv[1];
    scanner_args->start_port = atoi(argv[2]);
    scanner_args->end_port = atoi(argv[3]);

    validate_port(scanner_args->start_port);
    validate_port(scanner_args->end_port);

    if (scanner_args->end_port < scanner_args->start_port)
    {
        printf("Invalid port numbers. Starting port must be less than end port\n");
        exit(1);
    }
}

int scan_port(const char *ip, int port)
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_address.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported\n");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation error\n");
        return 1;
    }

    int connection = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    close(sockfd);
    return connection;
}

int main(int argc, char *argv[])
{
    ScannerArgs scanner_args;
    validate_args(argc, argv, &scanner_args);

    for (int i = scanner_args.start_port; i <= scanner_args.end_port; i++)
    {
        int connection = scan_port(scanner_args.ip, i);
        if (connection == 1)
        {
            printf("warn: error creating socket\n");
            exit(1);
        }

        if (connection == 0)
        {
            printf("%s:%d/TCP: connection success\n", scanner_args.ip, i);
        }
    }

    return 0;
}