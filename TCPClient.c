#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORTNUM 8080 //control channel
#define PORTNUM1 8070 //data channel
#define SA struct sockaddr
#define PATH_MAX 1024; 

void parseInput(char* buf[1024], char* input_array[100]);
void quit_cl(int *quit_toggle);
void ls_cl(char* input_array[100]);
void cd_cl(char* input_array[100]);
void sys_sr(int sockfd);
void get_sr(char* input_array[100]);

int main(){
	int sockfd, ser;
	char buf[1024];
	char buf2[1024];
	struct sockaddr_in tester;
	char * input_array[100];
	int quit_toggle = 0;
	char* error="User authentication required first";
	
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

		printf("\nMessage from client: ");
		fgets(buf, 1024, stdin);
		
		/*  ------------ QUIT ------------ */ 
		if(strncmp(buf,"QUIT", strlen("QUIT")) == 0){
			send(sockfd , buf , strlen(buf),0 ); 
			quit_cl(&quit_toggle);
		}
		else{
			/*  ------------ GET ------------ */
			if (strncmp(buf, "GET", strlen("GET")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				parseInput(&buf,input_array);
				//get_sr(input_array);
				continue;
        	}
			/*  ------------ PUT ------------ */
			if (strncmp(buf, "PUT", strlen("PUT")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				//put_ser(input_array, sockfd);
        	}
			/*  ------------ !LS ------------ */ 
        	if (strncmp(buf, "!LS", strlen("!LS")) == 0) {
				parseInput(&buf,input_array);
				ls_cl(input_array);
				continue;
        	}
			/*  ------------ !PWD ------------ */ 
			else if (strncmp(buf, "!PWD", strlen("!PWD")) == 0) {
				system("pwd");
				continue;
        	}
			/*  ------------ !CD ------------ */ 
			else if (strncmp(buf, "!CD", strlen("!CD")) == 0) {
				parseInput(&buf,input_array);
				cd_cl(input_array);
				continue;
			}
			/*  ------------ LS ------------ */
			if (strncmp(buf, "LS", strlen("LS")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				//sys_sr(sockfd);
				continue;
        	}
			/*  ------------ PWD ------------ */ 
			else if (strncmp(buf, "PWD", strlen("PWD")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				//sys_sr(sockfd);
				continue;
        	}
			/*  ------------ CD ------------ */ 
			else if (strncmp(buf, "CD", strlen("CD")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				//sys_sr(sockfd);
				continue;
			}
			else{
				send(sockfd , buf , strlen(buf) , 0 ); 
				read(sockfd, buf2, sizeof(buf2)); 
				printf("Message from server: %s\n",buf2); 
			}
		}
	}while (quit_toggle != 1);
	close (sockfd);
	return 0;
}

/* parse through user input and stores them in an array*/
void parseInput(char* buf[1024], char* input_array[100]){
  const char delimiter[2] = " ";
  char * input_tokens;
  int element_counter = 0;
  
  input_tokens = strtok(buf, delimiter);
  
  while( input_tokens != NULL ) {
    input_array[element_counter] = input_tokens;
    element_counter++ ;
    input_tokens = strtok(NULL, delimiter);
  }
  //input_array[1][strcspn(input_array[1], "\n")] = '\0';
}

/*calls sytem call for cd in the client*/
void quit_cl(int *quit_toggle){
	*quit_toggle = 1;
	printf("\nGoodbye!\n\n");
}

/*calls sytem call for cd in the client*/
void ls_cl(char* input_array[100]){
	char sys_string[1024];

	printf("> ls command recieved \n");

	if (input_array[1] != NULL && !isspace(input_array[1])){
		strncat(sys_string, "ls ", 100);
		strncat(sys_string, input_array[1], 100);
		puts(sys_string);

		if(input_array[2]!= NULL){
			strncat(sys_string, " ", 100);
			strncat(sys_string, input_array[2], 100);
			puts(sys_string);
		}
		system(sys_string);
	}
	else {
			system("ls");
	}
}

/*calls SYSTEM call for cd in the client*/
void cd_cl(char* input_array[100]){
	char* directory;

	if (input_array[1] != NULL){  //cd on its own should take you to the home directory 
		directory = input_array[1]; 
		directory[strlen(directory)-1] = '\0'; //adds the null character to the end of the parsed string			}
	}
	else{
		directory = "~\0";
	}

	printf("directory:%s" ,directory);
	printf("end of dir\n");

	int cd_status = chdir(directory); 

	if (cd_status == -1){
		printf("ERROR in !CD command\n");
	}
	else if(cd_status == 0){
		printf("Succesfully changed directories");
		system("pwd");
	}
}
