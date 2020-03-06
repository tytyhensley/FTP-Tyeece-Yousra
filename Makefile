all: server client 

server: TCPServer.c
	gcc TCPServer.c -o FTPserver

client: TCPClient.c
	gcc TCPClient.c -o FTPclient




