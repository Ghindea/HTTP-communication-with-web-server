# Client app for HTTP communication with web server

made by: _Daniel Ghindea - 325CB_

---

### Dependencies
The program uses [nlohman/json](#json-parsing) library for JSON parsing.

Also, during development, the server at 34.246.184.49 was used to test communications, but it may currently be down.

## MAKEFILE
```bash
    $ make run   # compile & run executable
    $ make clean # cleanup
```

## Description
Client app receives commands from standard input to interact with the web server. When a command
is given, the program sends an HTTP request to the server & receives a response that is displayed.

List of commands:
- `register` - create a user account
- `login`
- `enter_library` - request access to the library
- `get_books` - list a summary of all available books
- `get_book` - list the details of a specific book
- `add_book`
- `delete_book`
- `logout`
- `exit` - end the execution

### About
Program's functionalities are encapsulated in `client_http` class. Its arguments store client's `username`, `password` and `access token` and the principal method, `run_client()`,
implements an infinite loop where commands are read from standard input. Depending on the given input, additional data may be read and the method that corresponds to the action is called.

The functionalities for HTTP communication are implemented in `http_communication` class. It contains methods that compute `GET`, `POST` & `DELETE` requests and methods that realize the communication with the server through sockets.

Every action-related method work as it follows:
- a string that represents the `message` request is formed according to the desired action
- `send_to_server()` method is called with `message` as parameter
- the returned string is interpreted; if the HTTP request was successful a success message or the returned data is displayed, otherwise an error message is displayed.


### HTTP protocol
HTTP request protocol is implemented with the help of sockets. To request, update and delete data from the server `GET`, `POST` & `DELETE` methods are used.
- **register**: 
```json lines
POST /api/v1/tema/auth/register HTTP/1.1
Host: 34.246.184.49
Content-Type: application/json
Content-Length: 38

{"username":"README","password":"123"}
```

- **login**:
```json lines
POST /api/v1/tema/auth/login HTTP/1.1
Host: 34.246.184.49
Content-Type: application/json
Content-Length: 38

{"username":"README","password":"123"}
```
HTTP response from server contains a session cookie that is stored in its corresponding attribute while the program runs.

- **access**: 
```json lines
GET /api/v1/tema/library/access HTTP/1.1
Host: 34.246.184.49
Cookie: connect.sid=s%3AuAHUePO_ndfL6jfbpalzR7BEq07jxzIi.ucbnWjM6xVHLoy3QVoHPPVtZJYzfvs9Mqqw7zyTkXT8; Path=/; HttpOnly
```
HTTP response from server contains an access token that is stored in its corresponding attribute.

- **other commands** are implemented similarly by including the access token in the header `"Authorization: Bearer " + access_token` 

*check `define.h` for macros
*`get_JSON_credentials()` returns the credentials of the user as a string in JSON format (`{"username":"gigi","password":"1234"}`)

### JSON parsing
The program uses [nlohman/json](https://github.com/nlohmann/json) library for JSON parsing. The library is included in the project as a header file.
It was used to parse the JSON response from the server and extract the necessary information.

```c++
    rc = req.receive_from_server();
    json token = json::parse(strstr(rc, "{"));

    if (!token["token"].empty()) {
        access_token = token["token"];
        MSG(true, "Access granted");
    } else {
        MSG(false, "Access denied");
    }
```


### Checker
To run the checker run this command
```bash
    python3 checker/checker.py --script ALL ./client  # run all tests
    python3 checker/checker.py --help     # for more details
```

### Bibliography & tools
- [lab](https://pcom.pages.upb.ro/labs/lab9/http.html)
- [postman](https://www.postman.com/)