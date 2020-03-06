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

void parseInput(char* buffer[], char* input_array[]);
int user (int new_sock, char* input_array[]);
int pass(int status, int new_sock, char* input_array[]);

int main(){
	int listenfd, len, new_sock, cli, ser, status=0, stati, i, index=0;
	pid_t pid;
	struct sockaddr_in server, client;
	char buf[1024];
	fd_set read_fd_set;
	char* commands[2];
	char* errors[1024];
	char * input_array[100];

	//array of FTP comands
	commands[0]="USER";
	commands[1]="PASS";

	//array of user errors
	errors[0]="Command not found";
	errors[1]="User authentication required first";



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
		printf("sokect bind failed\n");
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
			if ((pid=fork())==0){//forks new proces to deal with client
				close(listenfd);

			do{
				bzero(buf, sizeof(buf));
				recv(new_sock, buf,sizeof(buf), 0);//reads the clients message
                printf("\nMessage From TCP client: "); 

                printf("%s\n",buf);

				cli=strncmp(buf,"bye", 4); //checks for exit message of client 

                parseInput(&buf,input_array);
  				

                index = 0;
                for (i=0; i<2; i++){
     				if (strncmp(input_array[0], commands[i],strlen(commands[i]) + 1 ) == 0){//checks for which basic command was entered by the user
          					index = i+1;
          					break;      
      				}
   			 	}


                switch(index) {//switched to the correct basic function
      				case 1: // USER - checks if user exists
        				status=user(new_sock, input_array);
        				break;
      				case 2://PASS - checks if username was authenticated first
        				if(status>0){ //authentication completed 
        					stati=pass(status, new_sock, input_array);
        				}

        				else{ //authentication failed 
        					write(new_sock,(const char*)errors[1], sizeof(buf));
        				}
        				
        				break;
        			default: //command not found
        				write(new_sock,(const char*)errors[0], sizeof(buf));
      			}


      		}while (cli!=0);
      		printf("socket-out");

      		close(new_sock);
      		exit(0);
			}
			close(new_sock);
		}
	}
}

// parse through user input and stores them in an array
void parseInput(char* buffer[1024], char* input_array[100]){
  const char delimiter[2] = " ";
  char * input_tokens;
  int element_counter = 0;

  input_tokens = strtok(buffer, delimiter);//tokens the input based on the null space separator
  
  while( input_tokens != NULL ) {
    input_array[element_counter] = input_tokens;//adds the separate tokens to an individual array slot
    element_counter++ ;
    input_tokens = strtok(NULL, delimiter);
  }
}

//Authorizee the username given by client
int user (int new_sock, char* input_array[100]){
	FILE *fp;
	char username[60];
	int stat=0;

	char* message1 ="File not found";
	char* message2 ="Username does not exist";
	char* message3 ="Username authorized, password required";

	fp = fopen("users.txt" , "r");//opens file for reading and makes sure it exists, retur -1 if not
   	if(fp == NULL) {
     write(new_sock,(const char*)message1, 1024);//writes error message to client if file does not exist. 
     return(-1);
   }

    while (fgets(username, sizeof(username), fp))
    {
    	stat++;

    	if (strncmp(input_array[1], username,strlen(username))==0) {//checks if the username matches any on file and returns the status 
			if (stat%2 != 0){//checks if the line number corressponds with a username
				write(new_sock,(const char*)message3, 1024);//send client message prompting for password
				fclose(fp);
				return(stat);
			}
			
		}
		
    }
    write(new_sock,(const char*)message2, 1024);//writes error message to client if username does not exist.
    fclose(fp);
    return(-1);
}

//Authorize the password given by client 
int pass(int status, int new_sock, char* input_array[100]){
	FILE *fp;
	char passwd[60];
	int i=0;

	char* message1 ="File not found";
	char* message2 ="Password was wrong or does not exists";
	char* message3 ="Password authorized, authentication complete";

	fp = fopen("users.txt" , "r");//opens file for reading and makes sure it exists, returns -1 if not
   	if(fp == NULL) {
     write(new_sock,(const char*)message1, 1024);//writes error message to client if file does not exist. 
     return(-1);
   	}

   while (fgets(passwd, sizeof(passwd), fp))
    {
    	i++;
    	if (i==(status+1)){
 
    		if (strncmp(input_array[1], passwd,strlen(passwd))==0) {//checks if the password matches the corresspoding one on file 
				write(new_sock,(const char*)message3, 1024);//send client message that authentiation is complete
				fclose(fp);
				return(0);
			}
    	}		
    }
    fclose(fp);
    write(new_sock,(const char*)message2, 1024);//writes error message to client if password does not exist.
    return(-1);

}
