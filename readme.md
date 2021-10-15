# Socket Programming in C 

__Network Programming__: Building a program that receives and transmits data between two or more computers by means of a network

__What is a socket?__ 
- Provided by the OS within devices
- Provides a high level abstraction so we don't have to worry about underlying physical and software details of networking.

## __5 Fundamental Steps in Socket Programming in C__
### 1. Socket:
- To create a socket (a Socket descriptor) we must call the `socket` function, which is provided by the header file. 

```
#include <sys/socket.h>
int sockfd = socket(int domain, int type, int protocol)
```

Let's look at these three arguments, `domain`, `type`, and `protocol`:

__domain__: integer, communication domain e.g., AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol)
__type__: communication type
SOCK_STREAM: TCP(reliable, connection oriented)
SOCK_DGRAM: UDP(unreliable, connectionless)
__protocol__: Protocol value for Internet Protocol(IP), which is 0. This is the same number which appears on protocol field in the IP header of a packet.(man protocols for more details)


...

### 2. Bind 
Binding is like registering a phone number to the phone - we need to bind the socket to a specific port number.
```
#include <sys/socket.h>
int bind(int sockfd, struct sockaddr *sockaddr, socklen_t addrlen)
```

This function takes the actual socket and binds it to a port address `sockaddr`.

Binding a request to TCP/IP indicates that it is ready to send and receive information. 

### 3. Listen
Defines how the server can listen for connection requests. More technically, this is the process of converting an unconnected socket into a passive socket, indicating that the kernel should accept incoming connection requests directed to this socket. 

```
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

After initiating the `listen` call, clients

### 4. Accept
This transitions the connection request from the `listen` method to an actual socket. After accepting the request, the data can be finally transferred between the nodes. 

```
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

This accepts the `sockfd` or socket file descriptor, the address, and the address length, which are the same arguments as the `bind` function. 

### 5. Connect
This is when the _client_ makes the connection request to the server. This involves the client creating a socket and connecting to a specific address. 

```
include <sys/socket.h> 

int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen);
```

In this case the `sockaddr` will be the IP address and port number of the server. 

# Attribution
Notes from [CSE4202](&list=PLZIwlOSv75K7jXcVABdIo3wyKp5NwXKlW&index=2https://www.youtube.com/watch?v=ws2uptjZwmA), [Socket Programming in C / C++](https://www.geeksforgeeks.org/socket-programming-cc/). Code example is sourced from [this tutorial](https://www.youtube.com/watch?v=nA0LU9sqJls) and [this blog post](https://www.binarytides.com/socket-programming-c-linux-tutorial/).