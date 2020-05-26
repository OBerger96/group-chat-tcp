# group-chat-client-server
Group chat program, using  socket programming in C , where there is a single server and multiple clients. The server forwards a client's message to all the other clients. The communication between the server and the clients is based on TCP Client/Server Communication.

## Getting Started
Copy the project to a local directory:
Clone with SSH 
```
git@github.com:OBerger96/group-chat-client-server.git
```
Clone with HTTPS
```
https://github.com/OBerger96/group-chat-client-server.git
```
## Platform
Linux with GCC compiler

## To Run the project
Make a directory.
```
mkdir name_dir
```
Put server.c in the directory.
```
mv from_dir/server.c to_dir/server.c
```
Put client.c in the directory.
```
mv from_dir/client.c to_dir/client.c
```
Compile server.c program.
```
gcc server.c -o server.out -pthread
```
Compile client.c program.
```
gcc client.c -o client.out -pthread
```
Run the server.
```
./server.out & port_number
```
Run the client.
```
./client.out username port_number
```
<b>Note:</b> The portnumber provided to clients should be same as the one given to server. Username is just an identity of yourself.

## Screenshot
![screenshot](https://github.com/OBerger96/group-chat-client-server/blob/master/images/group-chat-Client-Server0.jpg)

## Author
* **Omer Berger** - [group-chat-tcp](https://github.com/OBerger96/group-chat-tcp)
