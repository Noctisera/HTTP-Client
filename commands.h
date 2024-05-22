#ifndef COMMANDS_H
#define COMMANDS_H

#define IP_SERVER "34.246.184.49"
#define PORT_SERVER 8080 
#define SERVER "34.246.184.49:8080"
#define PAYLOAD "application/json"

#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define ENTER_LIBRARY "/api/v1/tema/library/access"
#define GET_BOOKS "/api/v1/tema/library/books"
#define GET_BOOK "/api/v1/tema/library/books/"
#define ADD_BOOK "/api/v1/tema/library/books"
#define DELETE_BOOK "/api/v1/tema/library/books/"
#define LOGOUT "/api/v1/tema/auth/logout"

// Return the number of arguments while parsing the buffer
int get_arguments(char *buffer, char **argv);

// Convert the arguments to a string
char *argv_to_string(char **argv, int start, int end);

// Register a user
void register_user(int sockfd);

// Login
void login(int sockfd, char *jwt, char **login_cookie);

// Get access to the library
void enter_library(int sockfd, char **jwt, char *login_cookie);

// Get all books from the library
void get_books(int sockfd, char *jwt, char *login_cookie);

// Get a book with a specific id from the library
void get_book(int sockfd, char *jwt, char *login_cookie);

// Add a book to the library
void add_book(int sockfd, char *jwt, char *login_cookie);

// Delete a book with a specific id from the library
void delete_book(int sockfd, char *jwt, char *login_cookie);

// Logout
void logout(int sockfd, char **jwt, char **login_cookie);

#endif