#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"
#include "parson.h"
#include "requests.h"
#include "commands.h"

int get_arguments(char *buf, char **argv) {
	int argc = 0;
	for (char *p = strtok(buf, " \t\n"); p; p = strtok(NULL, " \t\n"))
		argv[argc++] = p;

	return argc;
}

char *argv_to_string(char **argv, int start, int end) {
	static char str[BUFSIZ];
	strcpy(str, "");

	if (start >= end)
		return NULL;

	strcpy(str, argv[start]);

	for (int index = start + 1; index < end; ++index) {
		strcat(str, " ");
		strcat(str, argv[index]);
	}

	return str;
}

void register_user(int sockfd) {
	char buffer[BUFLEN];
	char username[BUFLEN];
	char password[BUFLEN];

	int argc;
	char *argv[BUFLEN];
	int condition = 1;

	printf("username=");
	fgets(buffer, BUFLEN, stdin);

	argc = get_arguments(buffer, argv);

	if (argc != 1) {
		condition = 0;
	}

	strcpy(username, argv[0]);

	printf("password=");
	fgets(buffer, BUFLEN, stdin);

	argc = get_arguments(buffer, argv);

	if (argc != 1) {
		condition = 0;
	}

	strcpy(password, argv[0]);

	if (condition == 0) {
		printf("EROARE - Numele de utilizator sau parola nu pot contine spatii!\n");
		return;
	}

	// Create the json
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	json_object_set_string(root_object, "username", username);
	json_object_set_string(root_object, "password", password);

	char *serialized_string = json_serialize_to_string_pretty(root_value);

	// Send the request
    char *message = compute_post_request(SERVER, REGISTER, PAYLOAD, serialized_string, NULL, NULL);

	send_to_server(sockfd, message);
	free(message);

	// Receive the response
	char *response = receive_from_server(sockfd);

	// Check if the request was successful
	if ((strstr(response, "error"))) {
		printf("EROARE - Inregistrare esuata!\n");
	} else {
		printf("SUCCES - Utilizator înregistrat cu succes!\n");
	}

	free(response);
	json_value_free(root_value);
	free(serialized_string);
}

void login(int sockfd, char *jwt, char **login_cookie) {
	char buffer[BUFLEN];
	char username[BUFLEN];
	char password[BUFLEN];

	int argc;
	char *argv[BUFLEN];
	int condition = 1;

	printf("username=");
	fgets(buffer, BUFLEN, stdin);

	argc = get_arguments(buffer, argv);

	if (argc != 1) {
		condition = 0;
	}

	strcpy(username, argv[0]);

	printf("password=");
	fgets(buffer, BUFLEN, stdin);

	argc = get_arguments(buffer, argv);

	if (argc != 1) {
		condition = 0;
	}

	strcpy(password, argv[0]);

	if (condition == 0) {
		printf("EROARE - Numele de utilizator sau parola nu pot contine spatii!\n");
		return;
	}

	// Create the json
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	json_object_set_string(root_object, "username", username);
	json_object_set_string(root_object, "password", password);

	char *serialized_string = json_serialize_to_string_pretty(root_value);

	// Send the request
    char *message = compute_post_request(SERVER, LOGIN, PAYLOAD, serialized_string, NULL, NULL);

	send_to_server(sockfd, message);
	free(message);

	// Receive the response
	char *response = receive_from_server(sockfd);

	// Check if the request was successful
	if (strstr(response, "{")) {
		printf("EROARE - Conectare esuata!\n");
	} else {
		// Get the login cookie
		char *aux = strtok(strstr(response, "connect.sid"), ";");
        free(*login_cookie);

        *login_cookie = (char *)malloc(strlen(aux) + 1);
        DIE(*login_cookie == NULL, "malloc failed");

        memcpy(*login_cookie, aux, strlen(aux) + 1);

		printf("SUCCES - Utilizator conectat cu succes!\n");
	}

	free(response);
	json_value_free(root_value);
	free(serialized_string);
}

void enter_library(int sockfd, char **jwt, char *login_cookie) {
	// Send the request
	char *message = compute_get_request(SERVER, ENTER_LIBRARY, login_cookie, NULL);

	send_to_server(sockfd, message);
	free(message);

	// Receive the response
	char *response = receive_from_server(sockfd);

	// Check if the request was successful
	if (strstr(response, "error")) {
        printf("EROARE - Acces in biblioteca respins!\n");
	} else {
		// Initialise the json root value with the response
		JSON_Value *json = json_parse_string(strstr(response, "{"));
		JSON_Object *root = json_value_get_object(json);

		// Get the jwt token
		char *aux = (char *)json_object_get_string(root, "token");
		free(*jwt);

		*jwt = (char *)malloc(strlen(aux) + 1);
		DIE(*jwt == NULL, "malloc failed");

		memcpy(*jwt, aux, strlen(aux) + 1);

		printf("SUCCES - Acces permis in biblioteca!\n");
	}

	free(response);
}

void get_books(int sockfd, char *jwt, char *login_cookie) {
	// Send the request
    char *message = compute_get_request(SERVER, GET_BOOKS, NULL, jwt);

	send_to_server(sockfd, message);
	free(message);

	// Receive the response
	char *response = receive_from_server(sockfd);

	if (strstr(response, "error")) {
        printf("EROARE - Primirea cartilor a esuat!\n");
	} else {
		printf("SUCCES - Utilizatorul are acces la carti!\n");

		// Initialise the json root value with the response
		JSON_Value *json = json_parse_string(strstr(response, "["));

		// Print the books
		char *books = json_serialize_to_string_pretty(json);
		printf("%s\n", books);
	}

	free(response);
}

void get_book(int sockfd, char *jwt, char *login_cookie) {
	char buffer[BUFLEN];
	char *id = (char *)malloc(BUFLEN);

	// Read the id
	printf("id=");
	fgets(buffer, BUFLEN, stdin);

	if (strspn(buffer, "0123456789") != strlen(buffer) - 1) {
		printf("EROARE - Id-ul cartii trebuie sa fie un numar!\n");
		return;
	}

	strcpy(id, buffer);
	id[strlen(id) - 1] = '\0'; // remove '\n' from id

	// Send the request
	char *path = (char *)malloc(strlen(GET_BOOK) + strlen(id) + 1);
	DIE(path == NULL, "malloc failed");

	strcpy(path, GET_BOOK);
	strcat(path, id);

	char *message = compute_get_request(SERVER, path, NULL, jwt);

	send_to_server(sockfd, message);
	free(message);
	free(path);

	// Receive the response
	char *response = receive_from_server(sockfd);

	if (strstr(response, "error")) {
		printf("EROARE - Cartea nu a fost primita!\n");
	} else {
		printf("SUCCES - Cartea a fost primita!\n");

		// Print the book
		JSON_Value *json = json_parse_string(strstr(response, "{"));
		printf("%s\n", json_serialize_to_string_pretty(json));
	}

	free(response);
}

void add_book(int sockfd, char *jwt, char *login_cookie) {
	char buffer[BUFLEN];

	printf("title=");
	fgets(buffer, BUFLEN, stdin);
	char *title = (char *)malloc(strlen(buffer) - 1);
	strcpy(title, buffer);
	title[strlen(title) - 1] = '\0'; // remove '\n' from title
	
	printf("author=");
	fgets(buffer, BUFLEN, stdin);
	char *author = (char *)malloc(strlen(buffer) - 1);
	strcpy(author, buffer);
	author[strlen(author) - 1] = '\0'; // remove '\n' from author

	printf("genre=");
	fgets(buffer, BUFLEN, stdin);
	char *genre = (char *)malloc(strlen(buffer) - 1);
	strcpy(genre, buffer);
	genre[strlen(genre) - 1] = '\0'; // remove '\n' from genre

	printf("publisher=");
	fgets(buffer, BUFLEN, stdin);
	char *publisher = (char *)malloc(strlen(buffer) - 1);
	strcpy(publisher, buffer);
	publisher[strlen(publisher) - 1] = '\0'; // remove '\n' from publisher

	// Make sure the page count is a number
	char *page_count = (char *)malloc(BUFLEN);
	printf("page_count=");
	fgets(buffer, BUFLEN, stdin);

	if (strspn(buffer, "0123456789") != strlen(buffer) - 1) {
		printf("EROARE - Numarul de pagini trebuie sa fie un numar!\n");
		free(title);
		free(author);
		free(genre);
		free(publisher);
		free(page_count);
		return;
	}

	// Make sure all read strings are not empty
	if (strlen(title) <= 1 || strlen(author) <= 1 || strlen(genre) <= 1 || strlen(publisher) <= 1) {
		printf("EROARE - Datele cartilor nu pot fi goale!\n");
		free(title);
		free(author);
		free(genre);
		free(publisher);
		free(page_count);
		return;
	}

	strcpy(page_count, buffer);
	memset(buffer, 0, BUFLEN);

	// Make the json for the book
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);

	json_object_set_string(root_object, "title", title);
	json_object_set_string(root_object, "author", author);
	json_object_set_string(root_object, "genre", genre);
	json_object_set_string(root_object, "publisher", publisher);
	json_object_set_number(root_object, "page_count", atoi(page_count));

	char *serialized_string = json_serialize_to_string_pretty(root_value);

	// Send the request
	char *message = compute_post_request(SERVER, ADD_BOOK, PAYLOAD, serialized_string, NULL, jwt);
	send_to_server(sockfd, message);
	free(message);

	// Receive the response
	char *response = receive_from_server(sockfd);

	if (strstr(response, "error")) {
		printf("EROARE - Cartea nu a fost adaugata!\n");
	} else {
		printf("SUCCES - Cartea a fost adaugata cu succes!\n");
	}

	free(title);
	free(author);
	free(genre);
	free(publisher);
	free(page_count);
	free(response);
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

void delete_book(int sockfd, char *jwt, char *login_cookie) {
	char buffer[BUFLEN];
	char id[BUFLEN];

	// Read the id
	printf("id=");
	fgets(buffer, BUFLEN, stdin);

	if (strspn(buffer, "0123456789") != strlen(buffer) - 1) {
		printf("EROARE - Id-ul cartii trebuie sa fie un numar!\n");
		return;
	}

	strcpy(id, buffer);
	id[strlen(id) - 1] = '\0'; // remove '\n' from id

	// Send the request
	char *path = (char *)malloc(strlen(DELETE_BOOK) + strlen(id) + 1);
	DIE(path == NULL, "malloc failed");

	strcpy(path, DELETE_BOOK);
	strcat(path, id);

	char *message = compute_delete_request(SERVER, path, NULL, jwt);

	send_to_server(sockfd, message);
	free(message);
	free(path);

	// Receive the response
	char *response = receive_from_server(sockfd);

	if (strstr(response, "error")) {
		printf("EROARE - Cartea cu id-ul %s nu a putut fi stearsa!\n", id);
	} else {
		printf("SUCCES - Cartea cu id-ul %s a fost stearsa cu succes!\n", id);
	}

	free(response);
}

void logout(int sockfd, char **jwt, char **login_cookie) {
	// Send the request
    char *message = compute_get_request(SERVER, LOGOUT, *login_cookie, NULL);

	send_to_server(sockfd, message);
	free(message);

	// Receive the response
	char *response = receive_from_server(sockfd);

	if (strstr(response, "{")) {
        printf("EROARE - Delogare esuata!\n");
	} else {
		printf("SUCCES - Delogare reusita!\n");
	}

	free(response);
	free(*jwt);
	free(*login_cookie);
	*jwt = NULL;
	*login_cookie = NULL;
}
