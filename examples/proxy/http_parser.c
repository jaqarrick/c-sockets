#include "http_parser.h"

// Function to parse HTTP request
int parse_http_request(const char *request, HttpRequest *http_request) {
    // Clear the HttpRequest struct
    memset(http_request, 0, sizeof(HttpRequest));
    
    // Temporary variables for parsing
    char buffer[BUFFER_SIZE];
    const char *method_end, *host_start, *host_end, *path_start, *path_end, *version_start, *version_end;
    const char *header_start, *header_end, *key_start, *key_end, *value_start, *value_end;

    // Parse the method (GET, POST, etc.)
    method_end = strchr(request, ' ');
    if (!method_end) return -1;
    strncpy(http_request->method, request, method_end - request);
    
    // Parse the path (in URL)
    path_start = method_end + 1;
    path_end = strchr(path_start, ' ');
    if (!path_end) return -1;
    strncpy(http_request->path, path_start, path_end - path_start);

    // Parse the version (HTTP/1.1 or similar)
    version_start = path_end + 1;
    version_end = strchr(version_start, '\r');
    if (!version_end) return -1;
    strncpy(http_request->version, version_start, version_end - version_start);

    // Parse host and port from the URL
    host_start = strstr(request, "Host: ");
    if (!host_start) return -1;
    host_start += 6;  // Skip "Host: "
    
    host_end = strchr(host_start, '\r');
    if (!host_end) return -1;
    strncpy(buffer, host_start, host_end - host_start);
    
    // Check if there's a port in the host (e.g., localhost:8080)
    char *colon = strchr(buffer, ':');
    if (colon) {
        *colon = '\0';  // Null-terminate the host before the colon
        http_request->port = atoi(colon + 1);
    } else {
        http_request->port = 80;  // Default HTTP port
    }
    strncpy(http_request->host, buffer, sizeof(http_request->host) - 1);
    
    // Parse headers
    header_start = strstr(request, "\r\n") + 2;  // Skip the first \r\n after the request line
    http_request->header_count = 0;

    while (header_start) {
        header_end = strstr(header_start, "\r\n");
        if(!header_end) break;
        
        // Look for empty line (end of headers)
        if (header_end == header_start) {
            break;
        }

        // Parse each header
        key_start = header_start;
        key_end = strchr(key_start, ':');
        if (!key_end) break;
        
        // Extract key and value
        int key_length = key_end - key_start;
        strncpy(http_request->headers[http_request->header_count][0], key_start, key_length);
        
        // Skip the space after the colon
        value_start = key_end + 1;
        value_end = header_end;
        int value_length = value_end - value_start;
        strncpy(http_request->headers[http_request->header_count][1], value_start, value_length);
        
        http_request->header_count++;
        
        // Move to the next header
        header_start = header_end + 2;
    }

    return 0;  // Successfully parsed
}
 
