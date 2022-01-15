# Basic TCP Server Client Demo
To try the demo, prepare the programs by running the `make` command. You can then run the socket server with `./server` and the client in another terminal window with `./client`. You can also run the client and server on two separate machines. On the server machine (this could be a VM, or remotely hosted machine) simply run `./server`. On the client, run `./client` but add the IP address of the server as an argument like so:
```
./client ip_addr
```
If an address isn't specified, the `client` program will default to localhost.