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
#define SA struct sockaddr 

void parseInput(char* buf[1024], char* input_array[100]);
void quit_cl(int *quit_toggle);
void ls_cl(char* input_array[100]);
void cd_cl(char* input_array[100]);
int get_sr(char* input_array[100], int sockfd);
int put_ser(char* input_array[100], int sockfd);

int main(int argc, char* argv[]){
	int sockfd, ser,port;
	char buf[1024];
	char buf2[1024];
	struct sockaddr_in tester;
	char * input_array[100];
	int quit_toggle = 0;
	char* error="User authentication required first";
	char*errorn="Command not found";
	
	memset(buf, 0, sizeof(buf));
	memset(buf2, 0, sizeof(buf2));

	if (argc !=3){
		printf("Wrong amount of arguments\n");
		printf("Corect syntax: ./client <server-ip> <server-port num>\n");
		printf("Check the READ ME for more informtion");
		exit (-1);
	}

	port=atoi(argv[2]);//changes argv[2] to an int

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0){
		printf ("socket creation failed \n");
		return (-1);
	}else{
		printf("socket created \n");
	}

	memset (&tester, 0, sizeof(tester)); //pads the address with zeros
	
	tester.sin_family= AF_INET; //sets address family
	//tester.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address
	tester.sin_port= htons(port);//sets the port number from command line
	if (inet_pton(AF_INET, argv[1], &tester.sin_addr) <= 0){//sets the ip address from the command line
    	printf("IP address invalid");
    	exit(-1);
    }

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
				parseInput(&buf,input_array);
				get_sr(input_array, sockfd);
				continue;
        	}
			/*  ------------ PUT ------------ */
			if (strncmp(buf, "PUT", strlen("PUT")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				parseInput(&buf,input_array);
				put_ser(input_array, sockfd);
				continue;
        	}
			/*  ------------ !LS ------------ */ 
        	if (strncmp(buf, "!LS", strlen("!LS")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2)); 
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				parseInput(&buf,input_array);
				ls_cl(input_array);
				continue;
        	}
			/*  ------------ !PWD ------------ */ 
			else if (strncmp(buf, "!PWD", strlen("!PWD")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2)); 
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				system("pwd");
				continue;
        	}
			/*  ------------ !CD ------------ */ 
			else if (strncmp(buf, "!CD", strlen("!CD")) == 0) {
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2)); 
				if(strncmp(buf2, error, strlen(error))==0){
					printf("Message from server: %s\n",buf2); 
					continue;
				}
				parseInput(&buf,input_array);
				cd_cl(input_array);
				continue;
			}
			/*  ------------ LS ------------ */
			if (strncmp(buf, "LS", strlen("LS")) == 0) {
				buf[strlen(buf)-1] = '\0';
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				printf("Message from server: %s\n",buf2);
				//memset(buf, 0, sizeof(buf2));
				//buf2[1024];
				bzero(buf2,sizeof(buf2));
				continue;
        	}
			/*  ------------ PWD ------------ */ 
			else if (strncmp(buf, "PWD", strlen("PWD")) == 0) {
				buf[strlen(buf)-1] = '\0';
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				printf("Message from server: %s\n",buf2); 
				//memset(buf, 0, sizeof(buf2));
				//buf2[1024];
				bzero(buf2,sizeof(buf2));
				continue;
        	}
			/*  ------------ CD ------------ */ 
			else if (strncmp(buf, "CD", strlen("CD")) == 0) {
				buf[strlen(buf)-1] = '\0';
				send(sockfd , buf , strlen(buf),0);
				read(sockfd, buf2, sizeof(buf2));
				printf("Message from server: %s\n",buf2);
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
 
}

/*quits the client*/
void quit_cl(int *quit_toggle){
	*quit_toggle = 1;
	printf("\nGoodbye!\n\n");
}

/*calls sytem call for ls in the client*/
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

/*calls sytem call for cd in the client*/
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

/*downloads a reqquested file from server*/
int get_sr(char* input_array[100], int sockfd){
	int file_socket, size;
	struct sockaddr_in tester1;
	FILE *fp;
	char* buffer[1000];
	char* buf2;
	char* buf[1000];
	char* error ="File not found";
	char* error1="User authentication required first";

	read(sockfd, buffer, sizeof(buffer));//reads from control channel
	if(strncmp(buffer, error1, strlen(error1))==0){
		printf("Message from server: %s\n",buffer); 
		return -1;
	}

	sleep(3);
	input_array[1][strcspn(input_array[1], "\n")] = '\0';//eliminates the trainling newline character at the end 

	/*Create data chanel*/
	if ((file_socket= socket(AF_INET, SOCK_STREAM, 0)) <0){
		printf ("socket creation failed \n");
		exit(0);
	}else{
		printf("socket created \n");
	}

	memset (&tester1, 0, sizeof(tester1)); //pads the address with zeros

	tester1.sin_family= AF_INET; //sets address family
	tester1.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address
	tester1.sin_port= htons(8070);//sets the port number 

	if (connect(file_socket, (SA*)&tester1, sizeof(tester1)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0);
    } 
    else{
        printf("connected to the server..\n"); 
    }
	
	read(file_socket, buf, sizeof(buf));//reads from data channel
	if(strncmp(buf, error, strlen(error))==0){
		printf("Message from server: %s\n",buf); 
		return -1;
	}
	size=atoi(buf);//changes buf into int
	printf("Server is sending file...");
	buf2=malloc(size);
	read(file_socket, buf2, size);//reads the file from buffer
	fp = fopen(input_array[1], "w");
	if(fp == NULL) {
		printf("File does not exists");//sends message to client that file does not exist
    	return(-1);
   	}
	fwrite(buf2, 1 ,size, fp );//writes buf2 to a ne local file
	free(buf2);
	fclose(fp);
	close(file_socket);//close data channel 
	return 0;
}

/*uploads a requested file to server*/
int put_ser(char* input_array[100], int sockfd){
	int file_socket, size;
	struct sockaddr_in tester2;
	FILE *fp;
	char* buf2;
	char* buffer[1000];
	char val[100];
	char* error="User authentication required first";
	char* error1 ="File not found";
	
	input_array[1][strcspn(input_array[1], "\n")] = '\0';//eliminates the trainling newline character at the end 

	read(sockfd, buffer, sizeof(buffer)); //reads from the control hannel 
	if(strncmp(buffer, error, strlen(error))==0){
		printf("Message from server: %s\n",buffer); 
		return -1;
	}

	sleep(3);
	/*Create data channel*/
	if ((file_socket= socket(AF_INET, SOCK_STREAM, 0)) <0){
		printf ("socket creation failed \n");
		exit(0);
	}else{
		printf("socket created \n");
	}

	memset (&tester2, 0, sizeof(tester2)); //pads the address with zeros

	tester2.sin_family= AF_INET; //sets address family
	tester2.sin_addr.s_addr= htonl(INADDR_ANY); //set any IP address
	tester2.sin_port= htons(8070);//sets the port number 

	if (connect(file_socket, (SA*)&tester2, sizeof(tester2)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0);
    } 
    else{
        printf("connected to the server..\n"); 
    }

	fp = fopen(input_array[1], "r");//opens file for reading and makes sure it exists, returns -1 if not
	if(fp == NULL) {
		write(file_socket,(const char*)error1, 1024);//sends message through data channel
		printf("File does not exist");//sends message to client that file does not exist
		close(file_socket);//closes data channel
    	return(-1);
   	}
	
	/* gets the size of file*/
	fseek(fp, 0, SEEK_END); 
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	buf2=malloc(size);//dynamically allocates memory for storing file
	
	int count=0;
	do{//reads them into a buffer
		char c =fgetc(fp);
		if (feof(fp)){
			break;
		}
		buf2[count]=c;
		count++;
	}while(1);

	sprintf(val, "%d", size); //changes size to character 

	write(file_socket,(const char*)val, sizeof(val));//sends size of file to client
	sleep(2);
	write(file_socket,(const char*)buf2, count);//sends file to client
	
	free(buf2);
	fclose(fp);
	close(file_socket);// close data channel
	return (0);
}