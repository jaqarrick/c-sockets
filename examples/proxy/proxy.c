#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "http_parser.h"

#define PORT 5100

void handle_client (int client_socket) {
	char buffer[1024];
	ssize_t read_size;
	read_size = recv(client_socket, buffer, sizeof(buffer) -1, 0);
	
	if(read_size <= 0) {
		perror("Error receiving client message\n");
		close(client_socket);
		return;
	}

	buffer[read_size] = '\0';
	HttpRequest request;
	
	if(parse_http_request(buffer, &request) < 0){
		printf("parse unsuccessfull\n");
		close(client_socket);
		return;
	}
	
	if(strcmp(request.method, "CONNECT") == 0){
		printf("Client requesting secure tunnel via https\n");
		// todo: implement https tunnel
		close(client_socket);
		return;
	}

	int proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(proxy_socket < 0){
		perror("Error creating proxy socket\n");
		close(proxy_socket);
		return;
	}
	struct sockaddr_in proxy_address;
	memset(&proxy_address, 0, sizeof(proxy_address));
	proxy_address.sin_family = AF_INET;
	proxy_address.sin_port = htons(request.port);
	
        struct addrinfo hints, *res;
    	int err;

    	// Zero out the hints structure
    	memset(&hints, 0, sizeof(hints));
    	hints.ai_socktype = SOCK_STREAM; // TCP socket
	
    	// Get address information for the host
		hints.ai_family = AF_INET; // Use AF_INET for IPv4
		err = getaddrinfo(request.host, NULL, &hints, &res);
    	if (err != 0) {
        	fprintf(stderr, "Error: %s\n", gai_strerror(err));
        	return;
    	}
    	// Print the first IP address associated with the host
    	char ip_str[INET6_ADDRSTRLEN];
    	if (res->ai_family == AF_INET) { // IPv4 address
        	struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        	inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, sizeof(ip_str));
        	printf("IPv4 Address: %s\n", ip_str);
    	} else if (res->ai_family == AF_INET6) { // IPv6 address
        	struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
        	inet_ntop(AF_INET6, &ipv6->sin6_addr, ip_str, sizeof(ip_str));
        	printf("IPv6 Address: %s\n", ip_str);
    	}

		// Copy the IP address to the proxy_address structure
		if (res->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
			proxy_address.sin_addr = ipv4->sin_addr;
		} else  {
			close(proxy_socket);
			close(client_socket);
			return;
		}

		// Free the address information
		freeaddrinfo(res);

	if(connect(proxy_socket, (struct sockaddr *)&proxy_address, sizeof(proxy_address)) < 0) {
		perror("Proxy connection failed\n");
		close(client_socket);
		close(proxy_socket);
		return;
	}

	send(proxy_socket, buffer, read_size, 0);
	read_size = recv(proxy_socket, buffer, sizeof(buffer), 0);
	send(client_socket, buffer, read_size, 0);

	close(proxy_socket);
	close(client_socket);
	return;
}

int main() {
	int server_sock, client_sock;
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(server_sock < 0){
		perror("Error creating server socket\n");
		printf("%d\n", server_sock);
		close(server_sock);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address, client_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if(bind(server_sock, (struct sockaddr*) &server_address, sizeof(server_address)) < 0){
		perror("Error binding server socket\n");
		close(server_sock);
		exit(EXIT_FAILURE);
	} else {
		printf("Server socket binded\n");
	}
	
	if(listen(server_sock, 1) < 0){
		perror("Failed to listen on server socket\n");
		close(server_sock);
		exit(EXIT_FAILURE);
	} else {
		printf("Server listening at port %d\n", PORT);
	}

	socklen_t client_len = sizeof(client_address);
	client_sock = accept(server_sock, (struct sockaddr*) &client_address, &client_len);
	if(client_sock < 0){
		perror("Failed to accept incoming client connection\n");
		close(client_sock);
		exit(EXIT_FAILURE);
	}

	printf("Client connected: %s\n", inet_ntoa(client_address.sin_addr));


	handle_client(client_sock);
	close(server_sock);
	exit(0);

}
