#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

#include "helpers.h"
#include "requests.h"


char *compute_get_request(const char *host, const char *url, char *cookies, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    DIE(message == NULL, "calloc failed");
    char *line = calloc(LINELEN, sizeof(char));
    DIE(line == NULL, "calloc failed");

    // Write the method name, URL, request params (if any) and protocol type
    sprintf(line, "GET %s HTTP/1.1", url);

    compute_message(message, line);

    // Add the host
	sprintf(line, "Host: %s", host);
	compute_message(message, line);

    // Add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
		memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookies);

        compute_message(message, line);
    }

    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);

        compute_message(message, line);
    }

    // Add final new line
    compute_message(message, "");
    
    free(line);

    return message;
}

char *compute_post_request(const char *host, const char *url, const char* content_type, 
                        const char *body_data, char *cookies, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    DIE(message == NULL, "calloc failed");
    char *line = calloc(LINELEN, sizeof(char));
    DIE(line == NULL, "calloc failed");
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    DIE(body_data_buffer == NULL, "calloc failed");

    // Write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Add the host
	sprintf(line, "Host: %s", host);
	compute_message(message, line);

    // Add headers and/or cookies, according to the protocol format
    memset(body_data_buffer, 0, LINELEN);
    strcat(body_data_buffer, body_data);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);
	

    // Add cookies
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookies);

        compute_message(message, line);
    }

    // Add token
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Add new line at end of header
	compute_message(message, "");

    // Add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);

    free(line);
    free(body_data_buffer);

    return message;
}

char *compute_delete_request(const char *host, const char *url, char *cookies, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    DIE(message == NULL, "calloc failed");
    char *line = calloc(LINELEN, sizeof(char));
    DIE(line == NULL, "calloc failed");

    // Write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookies);

        compute_message(message, line);
    }

    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);

        compute_message(message, line);
    }

    // Add final new line
    compute_message(message, "");
    
    free(line);

    return message;
}
