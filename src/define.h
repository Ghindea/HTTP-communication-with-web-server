#include "nlohman/json.hpp"

using namespace std;
using namespace nlohmann::literals;
using namespace nlohmann;

const string HOST = "34.246.184.49";
const string PORT = "8080";

const string REGISTER ="/api/v1/tema/auth/register";
const string LOGIN    = "/api/v1/tema/auth/login";
const string ACCESS   = "/api/v1/tema/library/access";
const string BOOK     = "/api/v1/tema/library/books";
const string LOGOUT   ="/api/v1/tema/auth/logout";

#define BUFFER_SIZE 4096
#define LINE_SIZE 1000

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

#define DIE(assertion, call_description)                                       \
  do {                                                                         \
    if (assertion) {                                                           \
      fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                       \
      perror(call_description);                                                \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define MSG(assertion, message)                                                \
  do {                                                                         \
    if (assertion) {                                                           \
        fprintf(stdout, "Success: %s \n", message);                            \
    } else {                                                                   \
        fprintf(stdout, "Error: %s \n",   message);                            \
    }                                                                          \
  } while (0)
