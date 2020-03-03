#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORTNUM 7

int main(){
	int sockfd, len, new_sock, cli, ser;
	struct sockaddr_in tester, client;
	char buf[1024]={0};
	char buf2[1024]={0};

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0){ //creates serve
		printf ("socket creation failed\n");
		return -1;
	}
	else{
		printf("socket created\n");
	}

	memset (&tester, 0, sizeof(tester)); //pads the address with zeros
	
	tester.sin_family= AF_INET; //sets address family
	tester.sin_port= htons(PORTNUM);//sets the port number 
	tester.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address

	

	if ((bind(sockfd, (struct sockaddr*)&tester,sizeof(tester))) != 0){  //binds server to a socket
		printf("sokect bind failed\n");
		return -1;
	}
	else{
		printf("bind completed\n");
	}

	if ((listen(sockfd, 5)) != 0) { 
        printf("listen failed\n"); 
        exit(0); 
    } 
    else
        printf("server listening\n"); 
    
	len =sizeof(client);

	new_sock = accept(sockfd, (struct sockaddr*)&client, &len);

	if (new_sock < 0){ //creates a new socet once a client connection is found
		printf("server acccept failed\n"); 
        return -1;
    }else{
    	printf("server accepted client \n");
    }

    read (new_sock, buf, strlen(buf)); //reads clients message

    do{
    	 printf("\nclient: %s",buf); 
    	 printf("\nserver: ");
    	 scanf("%s",buf2);

    	 write (new_sock, buf2, strlen(buf2)); //sends server message 

    	 listen (new_sock, 5); //listens for new client message 

    	 read (new_sock, buf, strlen(buf));

    	 cli=strcmp(buf,"bye"); //checks for exit message of client 
    	 ser=strcmp(buf2,"bye");

    } while ((cli!=0) || (ser!=0)); //closes loop once the exit message is sent

    close(sockfd);
    close(new_sock);

    return 0;
}