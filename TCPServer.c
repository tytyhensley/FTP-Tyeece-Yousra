#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h> 
#define STDIN 0
#define PORTNUM 8080

int main(){
	int listenfd, len, new_sock, cli, ser;
	pid_t pid;
	struct sockaddr_in server, client;
	char buf[1024];
	char buf2[1024];
	fd_set read_fd_set; 

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) <0){ //creates listening socket
		printf ("socket creation failed\n");
		return -1;
	}
	else{
		printf("socket created\n");
	}

	memset (&server, 0, sizeof(server)); //pads the address with zeros
	
	server.sin_family= AF_INET; //sets address family
	server.sin_port= htons(PORTNUM);//sets the port number 
	server.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address

	

	if ((bind(listenfd, (struct sockaddr*)&server,sizeof(server))) != 0){  //binds server to a socket
		printf("socket bind failed\n");
		return -1;
	}
	else{
		printf("bind completed\n");
	}

	if ((listen(listenfd, 5)) != 0) { //listens for client 
        printf("listen failed\n"); 
        exit(0); 
    } 
    else
        printf("server listening\n"); 
	
	while (1){
		FD_ZERO (&read_fd_set); // clears the descriptor set

		FD_SET(listenfd, &read_fd_set); //sets the listening socket in the read set

		select(listenfd+1, &read_fd_set, NULL, NULL, NULL); //selects a new ready descriptor

		if (FD_ISSET (listenfd, &read_fd_set)){ //accepts a new client connection if new descripter is given
			len =sizeof(client);
			new_sock = accept(listenfd, (struct sockaddr*)&client, &len);
			printf("new client connected!\n" );
			
			if ((pid=fork())==0){//forks new proces to deal with client
				close(listenfd);//closes the listening function in the child
				// bzero(buf, sizeof(buf)); 
				// bzero(buf2, sizeof(buf2)); 
						
					do{
							char buf[1024] = {0};
							char buf2[1024] = {0};

							read(new_sock, buf, sizeof(buf));//reads the clients message
			                printf("Message From TCP client: %s\n",buf); 
			               // puts(buf);
			                printf("Message from TCP server: ");
			                scanf("%s",buf2);


					    	cli=strncmp(buf,"bye", 2); //checks for exit message of client 
					    	ser=strncmp(buf2,"bye",2);

			                // write(new_sock, buf2, sizeof(buf2)); //sends message to the client
					    	send(new_sock , buf2 , strlen(buf2) , 0 ); 

			                //close(new_sock); 
			                //exit(0);


							}while ((cli!=0) || (ser!=0)); //closes loop once the exit message is sent
							printf("server-out");
							close(new_sock);
							exit(0);
		
			}

	}

	/*
    
	len =sizeof(client);

	new_sock = accept(sockfd, (struct sockaddr*)&client, &len);

	if (new_sock < 0){ //creates a new socet once a client connection is found
		printf("server acccept failed\n"); 
        return -1;
    }else{
    	printf("server accepted client \n");
    }*/
}
}
