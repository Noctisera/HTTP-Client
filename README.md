Pacuraru Macedoniu - Nicolae 325CA
# Tema 4 - PCom

This project contains a simple HTTP client implemented in C that interacts with an API. The client is designed to perform various actions such as user registration, login, accessing a library, managing books, and logout.

## Files Overview

### `client.c`

The `client.c` file contains the main function that serves as the entry point for the client application. It is responsible for establishing the connection to the server and processing user commands.

### `commands.c`

The `commands.c` file contains the implementations of the commands that the client can execute. Each function corresponds to a specific command and handles the details of sending requests to the server and processing responses.

#### Key Functions:
- `register_user`: Handles user registration by collecting username and password, creating a JSON payload, and sending a POST request to the server.
- `login`: Handles user login by collecting username and password, creating a JSON payload, and sending a POST request to the server. It also stores the session cookie for future requests.
- `enter_library`: Sends a GET request to enter the library, requiring a session cookie for access.
- `get_books`: Sends a GET request to retrieve a list of books, requiring a valid session token.
- `get_book`: Sends a GET request to retrieve details of a specific book by its ID.
- `add_book`: Collects book details from the user, creates a JSON payload, and sends a POST request to add a new book.
- `delete_book`: Sends a DELETE request to remove a book by its ID.
- `logout`: Sends a GET request to log out the user and clears the session cookie and token.

### `buffer.c`

The `buffer.c` file contains functions for managing a dynamic buffer, which is used to handle data received from the server.

### `helper.c`

The `helpers.c` file provides utility functions for network communication and error handling.

### `requests.c`

The `requests.c` file contains functions to construct different types of HTTP requests.

### `parson.c`

The `parson.c` file provides utility structs and functions for JSON parsing.

## Usage

### Running the Client

After compiling, run the client executable:

```sh
./client
```

The client will start and wait for user input. Enter the commands as listed in the command list to perform the respective actions.

### Example Commands

- Register a new user:

```makefile
register
username=test
password=test
```

Make

- Login as an existing user:

```makefile
login
username=test
password=test
```

- Enter the library:

```makefile
enter_library
```

- Get the list of books:

```makefile
get_books
```

- Get details of a specific book:

```makefile
get_book
id=1
```

- Add a new book:

```makefile
add_book
title=New Book
author=John Author
genre=Fiction
publisher=Best Publisher
page_count=300
```

- Delete a book:

```makefile
delete_book
id=1
```

- Logout:

```makefile
logout
```

- Exit the client:

```makefile
exit
```
