#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define PORTNUM 8080
#define SA struct sockaddr 

void parseInput();

	int sockfd, ser;
	char buf[1024];
	char buf2[1024];
	struct sockaddr_in tester;
	char * input_array[100];
	char sys_string[1024];
	char* directory;
	int quit_toggle = 0;



int main(){
	
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

  		//char buf[1024] = {0};
		// char buf2[1024] = {0};

	
		printf("\nMessage from client: ");
		fgets(buf, 1024, stdin);
		//write (sockfd, buf, strlen(buf));
		parseInput();

		// printf("\ncommand: %s" ,input_array[0]);
		// printf("command 2nd: %s\n\n" ,input_array[1]);

		/*  ------------ QUIT ------------ */ 
		if(strncmp(buf,"QUIT", strlen("QUIT")) == 0){
			quit_toggle = 1;
			printf("\n\nGoodbye!\n\n");
			//printf("%d", quit_toggle);
		}
		else{

		/*  ------------ !LS ------------ */ 
        if (strncmp(input_array[0], "!LS", strlen("!LS")) == 0) {
            printf("> ls command recieved \n");

			if (input_array[1] != NULL && !isspace(input_array[1])){
				strncat(sys_string, "ls ", 100);
				strncat(sys_string, input_array[1], 100);
				puts(sys_string);
				//!LS /Users/Yousra

				if(input_array[2]!= NULL){
				strncat(sys_string, " ", 100);
				strncat(sys_string, input_array[2], 100);
				//!LS -l /Users/Yousra
				puts(sys_string);
				}
				system(sys_string);
			}
			else {
				system("ls");
			}
        }

		/*  ------------ !PWD ------------ */ 
		else if (strncmp(input_array[0], "!PWD", strlen("!PWD")) == 0) {
            //printf("> ls command recieved \n");
			system("pwd");
        }

		/*  ------------ !CD ------------ */ 
		else if (strncmp(input_array[0], "!CD", strlen("!CD")) == 0) {

				// printf("\ncommand:%s\n" ,input_array[0]);
				// printf("command 2nd:%s" ,input_array[1]);
				// printf("command 3rd:%s" ,input_array[2]);
				// printf("end of command\n");
				
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
			//chdir(input_array[1]);

			if (cd_status == -1){
				printf("ERROR in !CD command\n");
			}
			else if(cd_status == 0){
				printf("Succesfully changed directories");
				system("pwd");
			}


		}


		else{
	    send(sockfd , buf , strlen(buf) , 0 );
		//puts(buf2);
		}
		
		read(sockfd, buf2, sizeof(buf2)); 
		printf("Message from server: %s\n",buf2); 
		ser=strncmp(buf2,"bye", 2);
		}

	}while (quit_toggle != 1);
		//}while (ser!=0 || quit_toggle != 1);

	
	//printf("client-out");
	close (sockfd);
	return 0;

}

void parseInput(){
  /* parse through user input and stores them in an array*/
  const char delimiter[2] = " ";
  char * input_tokens;
  int element_counter = 0;
  

  input_tokens = strtok(buf, delimiter);
  
  while( input_tokens != NULL ) {
    input_array[element_counter] = input_tokens;
    element_counter++ ;
    input_tokens = strtok(NULL, delimiter);
  }
}
