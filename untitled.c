#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

#define STDIN 0

int main( int argc, char * argv[]){


	fd_set read_fd_set;

	while(1){

		FD_ZERO (&read_fd_set);

		FD_SET(STDIN, &read_fd_set);

		select(STDIN+1, &read_fd_set, NULL, NULL, NULL);

		if (FD_ISSET (STDIN, &read_fd_set)){
			char buf[1024];
			memset(buf, 0, sizeof(buf));
			read(STDIN, buf, sizeof(buf));
			printf ("A key was pressed %s\n", buf);
		}

	}

}

