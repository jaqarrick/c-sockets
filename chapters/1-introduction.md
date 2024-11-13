# Introduction to Socket Programming 
Typical network applications consist of a client and server program. When each of these programs are executed a client and server process are initiated that communicate with eachother by reading from and writing to sockets. 

The first decision the developer must make when starting the build process is to decide whether the application should run over TCP or UDP. Let's look at the difference between the two:

**UDP** (User Datagram Protocol)
- Connectionless
- Sends independent packets of data from one system to another
- Does not guarantee delivery

**TCP** (Transmission Control Protocol)
- Connection oriented
- Reliable byte-stream channel two ways
