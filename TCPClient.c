#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORTNUM 8080
#define SA struct sockaddr 

int main(){
	int sockfd, ser;
	char buf[1024];
	char buf2[1024];
	struct sockaddr_in tester;

	 memset(buf, 0, sizeof(buf));
	 memset(buf2, 0, sizeof(buf2));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0){
		printf ("socket creation failed \n");
		return -1;
	}else{
		printf("socket created \n");
	}

	memset (&tester, 0, sizeof(tester)); //pads the address with zeros
	
	tester.sin_family= AF_INET; //sets address family
	tester.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address
	tester.sin_port= htons(PORTNUM);//sets the port number 

	

	if (connect(sockfd, (SA*)&tester, sizeof(tester)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else{
        printf("connected to the server..\n"); 
    }

    do{

    	char buf[1024] = {0};
		char buf2[1024] = {0};

	
		printf("Message from client: ");
		scanf("%s",buf);
		//write (sockfd, buf, strlen(buf));
	    send(sockfd , buf , strlen(buf) , 0 ); 

		read(sockfd, buf2, sizeof(buf2)); 
		printf("Message from server: %s\n",buf2); 
		//puts(buf2);

		ser=strncmp(buf2,"bye", 2);

	/*do{
		read (sockfd, buf2, strlen(buf2)); //reads 
		printf("\nserver: %s\n",buf2);
		printf("\nclient: ");
		scanf("%s",buf);
		ser=strcmp(buf2,"bye");
		write (sockfd, buf, strlen(buf));
	
	}while (ser!=0);*/

	}while (ser!=0);
	printf("client-out");
	close (sockfd);
	return 0;

}
