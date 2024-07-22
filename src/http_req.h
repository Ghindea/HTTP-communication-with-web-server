#include "define.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

typedef struct {
    char *data;
    size_t size;
} buffer;


buffer buffer_init(void)
{
    buffer buffer;

    buffer.data = NULL;
    buffer.size = 0;

    return buffer;
}

void buffer_destroy(buffer *buffer)
{
    if (buffer->data != NULL) {
        free(buffer->data);
        buffer->data = NULL;
    }

    buffer->size = 0;
}

int buffer_is_empty(buffer *buffer)
{
    return buffer->data == NULL;
}

void buffer_add(buffer *buffer, const char *data, size_t data_size)
{
    if (buffer->data != NULL) {
        buffer->data = (char *)realloc(buffer->data, (buffer->size + data_size) * sizeof(char));
    } else {
        buffer->data = (char *)calloc(data_size, sizeof(char));
    }

    memcpy(buffer->data + buffer->size, data, data_size);

    buffer->size += data_size;
}

int buffer_find(buffer *buffer, const char *data, size_t data_size)
{
    if (data_size > buffer->size)
        return -1;

    size_t last_pos = buffer->size - data_size + 1;

    for (size_t i = 0; i < last_pos; ++i) {
        size_t j;

        for (j = 0; j < data_size; ++j) {
            if (buffer->data[i + j] != data[j]) {
                break;
            }
        }

        if (j == data_size)
            return i;
    }

    return -1;
}

int buffer_find_insensitive(buffer *buffer, const char *data, size_t data_size)
{
    if (data_size > buffer->size)
        return -1;

    size_t last_pos = buffer->size - data_size + 1;

    for (size_t i = 0; i < last_pos; ++i) {
        size_t j;

        for (j = 0; j < data_size; ++j) {
            if (tolower(buffer->data[i + j]) != tolower(data[j])) {
                break;
            }
        }

        if (j == data_size)
            return i;
    }

    return -1;
}


#ifndef VAR2_HOMEWORK4_PUBLIC_HTTP_REQ_H
#define VAR2_HOMEWORK4_PUBLIC_HTTP_REQ_H


class http_req {
public:
    int sockfd;

    void open_connection(){
        struct sockaddr_in serv_addr;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        DIE(sockfd < 0, "socket");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(stoi(PORT));
        inet_aton(HOST.c_str(), &serv_addr.sin_addr);

        int rc = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        DIE(rc < 0, "connect");
    }

    void close_connection() {
        close(sockfd);
    }

    void compute_message(char *message, const char *line) {
        strcat(message, line);
        strcat(message, "\r\n");
    }
    char* GET(string url, string *param, char **cookies, int cookie_count, char **headers, int headers_count) {
        char *message = (char *) calloc(BUFFER_SIZE, sizeof(char));
        char *line    = (char *) calloc(LINE_SIZE, sizeof(char));

        if (param) {
            sprintf(line, "GET %s/%s HTTP/1.1", url.c_str(), param->c_str());
        } else {
            sprintf(line, "GET %s HTTP/1.1", url.c_str());
        }

        compute_message(message, line);

        memset(line, 0, LINE_SIZE);
        sprintf(line, "Host: %s", HOST.c_str());
        compute_message(message, line);

        if (cookies) {
            strcpy(line, "Cookie: ");
            for (int i = 0; i < cookie_count; i++) {
                strcat(line, cookies[i]);
                if (i != cookie_count - 1) strcat(line, "; ");
            }
            compute_message(message, line);
        }

        if (headers) {
            for (int i = 0; i < headers_count; i++) {
                memset(line, 0, LINE_SIZE);
                strcpy(line, headers[i]);
                compute_message(message, line);
            }
        }
        compute_message(message, "");

        return message;
    }

    char *POST(string url, char **body_data, int body_data_fields_count,
               char **cookies, int cookies_count, char **headers, int headers_count) {
        char *message = (char *) calloc(BUFFER_SIZE, sizeof(char));
        char *line    = (char *) calloc(LINE_SIZE, sizeof(char));
        char *body    = (char *) calloc(LINE_SIZE, sizeof(char));

        sprintf(line, "POST %s HTTP/1.1", url.c_str());
        compute_message(message, line);

        memset(line, 0, LINE_SIZE);
        sprintf(line, "Host: %s", HOST.c_str());
        compute_message(message, line);

        for (int i = 0; i < body_data_fields_count; i++) {
            if (i != 0) strcat(body, "&");
            strcat(body, body_data[i]);
        }

        memset(line, 0, LINE_SIZE);
        sprintf(line, "Content-Type: application/json");
        compute_message(message, line);

        memset(line, 0, LINE_SIZE);
        sprintf(line, "Content-Length: %ld", strlen(body));
        compute_message(message, line);

        if (cookies) {
            strcpy(line, "Cookie: ");
            for (int i = 0; i < cookies_count; i++) {
                strcat(line, cookies[i]);
                if (i != cookies_count - 1) strcat(line, "; ");
            }
            compute_message(message, line);
        }
        if (headers) {
            for (int i = 0; i < headers_count; i++) {
                memset(line, 0, LINE_SIZE);
                strcpy(line, headers[i]);
                compute_message(message, line);
            }
        }
        compute_message(message, "");

        strcat(message, body);

        free(line);

        return message;
    }

    char *DELETE(string url, string *param, char **cookies, int cookie_count, char **headers, int headers_count) {
        char *message = (char *) calloc(BUFFER_SIZE, sizeof(char));
        char *line    = (char *) calloc(LINE_SIZE, sizeof(char));

        if (param) {
            sprintf(line, "DELETE %s/%s HTTP/1.1", url.c_str(), param->c_str());
        } else {
            sprintf(line, "DELETE %s HTTP/1.1", url.c_str());
        }

        compute_message(message, line);

        memset(line, 0, LINE_SIZE);
        sprintf(line, "Host: %s", HOST.c_str());
        compute_message(message, line);

        if (cookies) {
            strcpy(line, "Cookie: ");
            for (int i = 0; i < cookie_count; i++) {
                strcat(line, cookies[i]);
                if (i != cookie_count - 1) strcat(line, "; ");
            }
            compute_message(message, line);
        }

        if (headers) {
            for (int i = 0; i < headers_count; i++) {
                memset(line, 0, LINE_SIZE);
                strcpy(line, headers[i]);
                compute_message(message, line);
            }
        }
        compute_message(message, "");

        return message;
    }

    void error(const char *msg) {
        perror(msg);
        exit(0);
    }

    void send_to_server(char *message) {
        int bytes, sent = 0;
        int total = strlen(message);

        do {
            bytes = write(sockfd, message + sent, total - sent);
            if (bytes < 0) {
                error("ERROR writing message to socket");
            }

            if (bytes == 0) {
                break;
            }

            sent += bytes;
        } while (sent < total);
    }

    char *receive_from_server() {
        char response[BUFFER_SIZE];
        buffer buffer = buffer_init();
        int header_end = 0;
        int content_length = 0;

        do {
            int bytes = read(sockfd, response, BUFFER_SIZE);

            if (bytes < 0) {
                error("ERROR reading response from socket");
            }

            if (bytes == 0) {
                break;
            }

            buffer_add(&buffer, response, (size_t)bytes);

            header_end =
                    buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

            if (header_end >= 0) {
                header_end += HEADER_TERMINATOR_SIZE;

                int content_length_start =
                        buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);

                if (content_length_start < 0) {
                    continue;
                }

                content_length_start += CONTENT_LENGTH_SIZE;
                content_length = strtol(buffer.data + content_length_start, NULL, 10);
                break;
            }
        } while (1);
        size_t total = content_length + (size_t)header_end;

        while (buffer.size < total) {
            int bytes = read(sockfd, response, BUFFER_SIZE);

            if (bytes < 0) {
                error("ERROR reading response from socket");
            }

            if (bytes == 0) {
                break;
            }

            buffer_add(&buffer, response, (size_t)bytes);
        }
        buffer_add(&buffer, "", 1);
        return buffer.data;
    }

};


#endif //VAR2_HOMEWORK4_PUBLIC_HTTP_REQ_H
