#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096
#define MAX_HEADERS 50

typedef struct {
    char method[16];
    char host[256];
    int port;
    char path[1024];
} HttpRequest;

int parse_http_request(const char *request, HttpRequest *http_request);

#endif // HTTP_PARSER_H
