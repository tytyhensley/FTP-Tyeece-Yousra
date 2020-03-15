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
#define PATH_MAX 1024

void parseInput(char* buffer[], char* input_array[]);
void quit (int* cli);
int user (int new_sock, char* input_array[]);
int pass(int status, int new_sock, char* input_array[]);
int getfp(int status,  char* input_array[], int new_sock);
int putfp(int status,  char* input_array[], int new_sock);
int popencmds(int stat, int new_sock, char* command[]);
int cd_server(int status,  char* input_array[], int new_sock);

int main(){
	int listenfd, len, new_sock, cli=1, ser, status=0, stati, i, index=0;
	pid_t pid;
	struct sockaddr_in server, client;
	char buf[1024];
	fd_set read_fd_set;
	char* commands[1024];
	char* errors[1024];
	char * input_array[100];


	//array of FTP comands
	commands[0]="USER";
	commands[1]="PASS";
	commands[2]="GET";
	commands[3]="PUT";
	commands[4]="LS";
	commands[5]="CD";
	commands[6]="PWD";
	commands[7]="QUIT";
	commands[8]="!LS";
	commands[9]="!PWD";
	commands[10]="!CD";


	//array of user errors
	errors[0]="Command not found";
	errors[1]="User authentication required first";
	errors[2]="0";


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
					recv(new_sock, buf, sizeof(buf), 0);//reads the clients message
                	printf("\nMessage From TCP client: "); 

                	printf("%s\n",buf);

                	parseInput(&buf,input_array);

                	index = 0;
                	for (i=0; i<11; i++){
     					if (strncmp(input_array[0], commands[i],strlen(commands[i])+1) == 0){//checks for which basic command was entered by the user
          					index = i+1;
          					break;      
      					}
   			 		}

					printf("%d", index);
                	switch(index) {//switched to the correct basic function
      					case 1: // USER - checks if user exists
        					status=user(new_sock, input_array);
        				break;
      					case 2://PASS - checks if password is correct
        					if(pass(status, new_sock, input_array)==1){
        						write(new_sock,(const char*)errors[1], sizeof(buf));
        					}
        				break;
        				case 3://GETFP - uploads a file to the clients
        					if(getfp(status, input_array, new_sock)==1){
        						write(new_sock,(const char*)errors[1], sizeof(buf));
        					}
        				break;
						case 4://PUTFP - downloads a file from the clients
        					if(putfp(status, input_array, new_sock)==1){
        						write(new_sock,(const char*)errors[1], sizeof(buf));
        					}
        				break;
						case 5://LS - system calls ls
        					if(popencmds(status, new_sock, "ls")==1){
        						write(new_sock,(const char*)errors[1], sizeof(buf));
        					}
        				break;
						case 6://CD - system calls cd
        					if(cd_server(status, input_array, new_sock)==1){
        						write(new_sock,(const char*)errors[1], sizeof(buf));
        					}
        				break;
						case 7://PWD - system calls pwd
        					if(popencmds(status, new_sock, "pwd")==1){
        						write(new_sock,(const char*)errors[1], sizeof(buf));
        					}
        				break;
						case 8://QUIT - exits the socket
							printf("closing socket....");
							quit(&cli);
        				break;

						case 9:
						case 10:
						case 11:
							if (status<=0){
								write(new_sock,(const char*)errors[1], sizeof(buf));
							}
							else{
								write(new_sock,(const char*)errors[2], sizeof(buf));
							}
        				break;
        				default: //command not found
        					write(new_sock,(const char*)errors[0], sizeof(buf));
      				}


      			}while (cli!=0);
      			printf("socket-out\n");
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
  input_array[1][strcspn(input_array[1], "\n")] = '\0';//deletes trailing character from the end of tokenss
}

//quits the socket
void quit (int* cli){
	*cli=0;
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
		username[strcspn(username, "\n")] = '\0';//omits the trailing newline character from username
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
	if(status<0){//checcks ifuser has been authenticated
		return(1);
	}

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
			passwd[strcspn(passwd, "\n")] = '\0';//omits the trailing newline character from passwd	
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

//Uploads requested file to client
int getfp(int status,  char* input_array[100], int new_sock){
	if(status<=0){//checcks if user has been authenticated
		return(1);
	}

	int filefd, file_sock, len, size;
	struct sockaddr_in server1, client1;
	FILE *fp;
	char* buffer;
	char val[1000];
	char* message1 ="File not found";
	char* msg2="0";
	
	write(new_sock,(const char*)msg2,1024);//sends message through control channel
	
	/*Creates the data channel*/
	if ((filefd = socket(AF_INET, SOCK_STREAM, 0)) <0){ //creates listening socket
		printf ("socket creation failed\n");
		return -1;
	}
	else{
		printf("socket created\n");
	}

	memset (&server1, 0, sizeof(server1)); //pads the address with zeros
	
	server1.sin_family= AF_INET; //sets address family
	server1.sin_port= htons(8070);//sets the port number 
	server1.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address

	if ((bind(filefd, (struct sockaddr*)&server1,sizeof(server1))) != 0){  //binds server to a socket
		printf("sokect bind failed\n");
		return -1;
	}
	else{
		printf("bind completed\n");
	}
	

	if ((listen(filefd, 5)) != 0) { //listens for client 
        printf("listen failed\n"); 
        exit(0); 
    } 
    else
        printf("server listening\n"); 
	
	
	len =sizeof(client1);
	file_sock = accept(filefd, (struct sockaddr*)&client1, &len);

	fp = fopen(input_array[1], "r");//opens file for reading and makes sure it exists, returns -1 if not
	if(fp == NULL) {
		write(file_sock,(const char*)message1,1024);//sends message to client that file does not exist through data channel
    	close(file_sock);
		close(filefd);
		return(-1);
   	}
	
	/* gets the size of file*/
	fseek(fp, 0, SEEK_END); 
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	buffer=malloc(size);//dynamically allocates memory for storing file
	int count=0;
	do{//reads them into a buffer
		char c =fgetc(fp);
		if (feof(fp)){
			break;
		}
		buffer[count]=c;
		count++;
	}while(1);

	sprintf(val, "%d", size); //changes size to character 

	write(file_sock,(const char*)val, sizeof(val));//sends size of file to client
	sleep(2);
	write(file_sock,(const char*)buffer, count);//sends file to client
	
	free(buffer);
	fclose(fp);
	close(file_sock);// close data channel
	close(filefd);
	return (0);
}

//Dowmloads requested file from server
int putfp(int status,  char* input_array[100], int new_sock){
	if(status<=0){//checcks if user has been authenticated
		return(1);
	}

	int filefd1, file_sock1, len, size;
	struct sockaddr_in server2, client2;
	FILE *fp;
	char* buffer;
	char* buf[1000];
	char val[1000];
	char* msg2="0";
	char* error1 ="File not found";
	
	write(new_sock,(const char*)msg2,1024);//send message through control channel
	
	/*Creates the data channel*/
	if ((filefd1 = socket(AF_INET, SOCK_STREAM, 0)) <0){ //creates listening socket
		printf ("socket creation failed\n");
		return -1;
	}
	else{
		printf("socket created\n");
	}

	memset (&server2, 0, sizeof(server2)); //pads the address with zeros
	
	server2.sin_family= AF_INET; //sets address family
	server2.sin_port= htons(8070);//sets the port number 
	server2.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address

	if ((bind(filefd1, (struct sockaddr*)&server2,sizeof(server2))) != 0){  //binds server to a socket
		printf("socket bind failed\n");
		close(filefd1);
		return -1;
	}
	else{
		printf("bind completed\n");
	}
	
	if ((listen(filefd1, 5)) != 0) { //listens for client 
        printf("listen failed\n"); 
		close(filefd1);
        exit(0); 
    } 
    else
        printf("server listening\n"); 

	len =sizeof(client2);
	file_sock1 = accept(filefd1, (struct sockaddr*)&client2, &len);

	read(file_sock1, buf, sizeof(buf));//reads from data channel
	if(strncmp(buf, error1, strlen(error1))==0){
		printf("Message from client: %s\n",buf); 
		close(file_sock1);
		close(filefd1);
		return -1;
	}
	size=atoi(buf);//changes buf into int
	buffer=malloc(size);
	read(file_sock1, buffer, size);//reads the file from stock
	fp = fopen(input_array[1], "w");
	if(fp == NULL) {
		printf("File does not exists");//sends message to client that file does not exist
		close(file_sock1);
		close(filefd1);
    	return(-1);
   	}
	fwrite(buffer, 1 ,size, fp );//writes the buffer into a new local file 
	
	free(buffer);
	fclose(fp);
	close(file_sock1);
	close(filefd1);//close data channel 
	return 0;
}

//Peforms system commands requested from client
int popencmds(int stat, int new_sock, char* command[100]){
	if(stat<=0){//checcks if user has been authenticated
		return(1);
	}
	
	FILE *fp_ls;
    int status;
    char* path[PATH_MAX];
	char* message1="Error, wrong command line usage";

    fp_ls = popen(command, "r");
    if(fp_ls == NULL) {
        write(new_sock,(const char*)message1, 1024);//sends error message to client.
		return -1;
    }

    while (fgets(path, sizeof(path), fp_ls) != NULL){
        printf("%s", path);
        send(new_sock, (const char*)path , strlen(path) , 0);
    }
    status = pclose(fp_ls);

    if (status == -1) {
        printf("\n\nERROR on file close\n");
    }
    else{
        printf("\nfile successfully closed \n");

     }
}

//Performs CD commnad if requested from client
int cd_server(int status,  char* input_array[100], int new_sock){
	if(status<=0){//checcks ifuser has been authenticated
		return(1);
	}
	
	char* response ="Successfully executed"; 
    char* cd_error = "ERROR in CD command"; 

	int cd_status = chdir(input_array[1]); 

     if (cd_status == -1){//checks if the command was a success or failure
    	printf("ERROR in CD command\n");
        write(new_sock,(const char*)cd_error, 1024);//writes error message to client if file does not exist. 
		return -1;

    }
    else if(cd_status == 0){
        printf("Succesfully changed directories\n");
        system("pwd");
        write(new_sock,(const char*)response, 1024);//writes error message to client if file does not exist. 
	}
	return 0;
}