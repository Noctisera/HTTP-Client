#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include "commands.h"

int main(int argc, char *argv[])
{
    char buffer[BUFLEN];
	char *jwt = NULL;
	char *login_cookie = NULL;
    int sockfd;


    while (1) {
		sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

		memset(buffer, 0, BUFLEN);
		fgets(buffer, BUFLEN, stdin);

		// Get the command and the arguments
		char *argv[BUFLEN];
		int argc = 0;
		for (char *p = strtok(buffer, " \t\n"); p; p = strtok(NULL, " \t\n")) {
			argv[argc++] = p;
		}

		// Check if the command is valid
		if (argc != 1) {
			printf("Invalid command\n");
			continue;
		}

		char *command = argv[0];

		if (strcmp(command, "register") == 0) {
			register_user(sockfd);
		} else if (strcmp(command, "login") == 0) {
			login(sockfd, jwt, &login_cookie);
		} else if (strcmp(command, "enter_library") == 0) {
			enter_library(sockfd, &jwt, login_cookie);
		} else if (strcmp(command, "get_books") == 0) {
			get_books(sockfd, jwt, login_cookie);
		} else if (strcmp(command, "get_book") == 0) {
			get_book(sockfd, jwt, login_cookie);
		} else if (strcmp(command, "add_book") == 0) {
			add_book(sockfd, jwt, login_cookie);
		} else if (strcmp(command, "delete_book") == 0) {
			delete_book(sockfd, jwt, login_cookie);
		} else if (strcmp(command, "logout") == 0) {
			logout(sockfd, &jwt, &login_cookie);
		} else if (strcmp(command, "exit") == 0) {
			close_connection(sockfd);
			break;
		} else {
			printf("Invalid command\n");
		}
	}

	free(jwt);

    return 0;
}
