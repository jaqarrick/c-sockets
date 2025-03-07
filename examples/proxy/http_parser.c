#include "http_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int parse_http_request(const char *raw_request, HttpRequest *http_request)
{
    memset(http_request, 0, sizeof(HttpRequest));
    char method[10] = {0};
    char path[1024] = {0};
    char host[256] = {0};
    int port = 80;
    
    // make a copy of the request for parsing
    char *request_copy = strdup(raw_request);
    if (!request_copy) {
        fprintf(stderr, "Parser: Memory allocation failed\n");
        return -1;
    }
    
    char *saveptr1 = NULL;
    char *saveptr2 = NULL;
    
    // parse first line
    char *line = strtok_r(request_copy, "\r\n", &saveptr1);
    if (line) {
        char *token = strtok_r(line, " ", &saveptr2);
        if (token) {
            // method
            strncpy(method, token, sizeof(method) - 1);
            token = strtok_r(NULL, " ", &saveptr2);
            if (token) {
                strncpy(path, token, sizeof(path) - 1);
            }
        }
        
        // Parse headers
        while ((line = strtok_r(NULL, "\r\n", &saveptr1)) != NULL) {
            // look for Host header
            if (strncasecmp(line, "Host:", 5) == 0) {
                char *host_value = line + 5;
                // skip whitespace
                while (isspace(*host_value)) host_value++;
                
                // parse host and port if present
                char *colon = strchr(host_value, ':');
                if (colon) {
                    // host with port
                    size_t host_len = colon - host_value;
                    strncpy(host, host_value, host_len);
                    host[host_len] = '\0';
                    port = atoi(colon + 1);
                } else {
                    // host without port
                    strncpy(host, host_value, sizeof(host) - 1);
                }
                break;
            }
        }
    }
    
    free(request_copy);

    if (strlen(path) == 0 || strlen(host) == 0 || strlen(method) == 0)
    {
        fprintf(stderr, "Parser: Path, host, or method not determined\n");
        return -1;
    }

    strncpy(http_request->method, method, sizeof(http_request->method) - 1);
    strncpy(http_request->host, host, sizeof(http_request->host) - 1);
    strncpy(http_request->path, path, sizeof(http_request->path) - 1);
    http_request->port = port;

    return 0;
}
