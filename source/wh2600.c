#include "wh2600.h"

#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <assert.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "reporter_error.h"

#define HTTP_OK \
"HTTP/1.1 200 OK\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Content-Length: 7 \r\n\
\r\n\
success"

#define HTTP_BAD \
"HTTP/1.1 400 Bad Request\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Content-Length: 11 \r\n\
\r\n\
bad request"

typedef struct {
    int server_fd;
    reporter_error *error;
    unsigned finished;
    wh2600_response response;
} await_connection_params;


static char _interrupt = 0;

void wh2600_handle_sigint(int signal) {
    if(signal != SIGINT)
        return;
    _interrupt = 1;
}

wh2600_response parse_request(int client, reporter_error *error) {
    char buffer[1024] = {0};
    size_t recieved = recv(client, buffer, 1024, 0);
    if (recieved < 0) {
        if (error) {
            error->kind = REPORTER_COMMUNCATION_ERROR;
            error->message = "weather server unexpected eof";
        }
        close(client);
        return (wh2600_response) {0};
    }

    regmatch_t groupArray[2] = {0};
    int err;
    regex_t regex;
    //URL
    err = regcomp(&regex, "^GET (\\/.*) HTTP", REG_EXTENDED);
    assert(err == 0);

    if(regexec(&regex, buffer, 2, groupArray, 0) == REG_NOMATCH) {
        if (error) {
            error->kind = REPORTER_COMMUNCATION_ERROR;
            error->message = "request url failed to parse";
        }
        char response[] = HTTP_BAD;
        write(client, &response, strlen(response));
        close(client);
        regfree(&regex);
        return (wh2600_response) {0};
    }
    regfree(&regex);

    char input[1024] = {0};
    memcpy(input, buffer+groupArray[1].rm_so, groupArray[1].rm_eo-groupArray[1].rm_so);

    // humidity
    err = regcomp(&regex, "[\\&|\\?]humidity=([0-9]*)\\&", REG_EXTENDED);
    assert(err == 0);

    if(regexec(&regex, input, 2, groupArray, 0) == REG_NOMATCH) {
        if (error) {
            error->kind = REPORTER_COMMUNCATION_ERROR;
            error->message = "weather server missing humidity info in request";
        }
        char response[] = HTTP_BAD;
        write(client, &response, strlen(response));
        close(client);
        regfree(&regex);
        return (wh2600_response) {0};
    }
    regfree(&regex);

    memset(buffer, 0, 1024);
    memcpy(buffer, input+groupArray[1].rm_so, groupArray[1].rm_eo - groupArray[1].rm_so);
    int humidity = atoi(buffer);

    // temperature

    err = regcomp(&regex, "[\\&|\\?]tempf=([0-9.]*)\\&", REG_EXTENDED);
    assert(err == 0);

    if(regexec(&regex, input, 2, groupArray, 0) == REG_NOMATCH) {
        if (error) {
            error->kind = REPORTER_COMMUNCATION_ERROR;
            error->message = "weather server missing temperature info in request";
        }
        char response[] = HTTP_BAD;
        write(client, &response, strlen(response));
        close(client);
        regfree(&regex);
        return (wh2600_response) {0};
    }
    regfree(&regex);

    memset(buffer, 0, 1024);
    memcpy(buffer, input+groupArray[1].rm_so, groupArray[1].rm_eo - groupArray[1].rm_so);
    double temperature = atof(buffer);
    double celcius = (temperature - 32) * 5/9;

    char response[] = HTTP_OK;
    write(client, &response, strlen(response));
    close(client);

    if (error) {
        error->kind = REPORTER_NO_ERROR;
        error->message = "ok";
    }

    return (wh2600_response) {
        .humidity = humidity,
        .temperature = celcius,
    };
}

void* await_connection(void * params) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    await_connection_params* p = params;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = 0;
    if ((client_fd = accept(p->server_fd, (struct sockaddr*)&client_addr, &client_addr_len)) < 0) {
        if (p->error) {
            p->error->kind = REPORTER_CONNECTION_ERROR;
            p->error->message = strerror(errno);
        }
        return NULL;
    }
    p->response = parse_request(client_fd, p->error);
    p->finished = 1;
    return NULL;
}

wh2600_response wh2600_query(uint64_t timeout, uint16_t port, reporter_error *error) {
    signal(SIGINT, wh2600_handle_sigint);
    int fd;

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        if (error) {
            error->kind = REPORTER_CONNECTION_ERROR;
            error->message = strerror(errno);
        }
        return (wh2600_response) {0};
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port),
    };

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    if(bind(fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        if (error) {
            error->kind = REPORTER_CONNECTION_ERROR;
            error->message = strerror(errno);
        }
        return (wh2600_response) {0};
    }

    if(listen(fd, 1) < 0) {
        if (error) {
            error->kind = REPORTER_CONNECTION_ERROR;
            error->message = strerror(errno);
        }
        return (wh2600_response) {0};
    }

    pthread_t awaiter = {0};
    await_connection_params p = {
        .server_fd = fd,
        .error = error,
        .finished = 0,
        .response = {0},
    };

    pthread_create(&awaiter, NULL, await_connection, (void*) &p);
    pthread_detach(awaiter);
    int end = time(NULL) + timeout;

    while(1) {
        if (p.finished) {
            break;
        }
        if ((time(NULL) >= end) || _interrupt) {
            if (error) {
                error->kind = REPORTER_COMMUNCATION_ERROR;
                error->message = "weather server ran for too long or was interrupted";
            }

            pthread_cancel(awaiter);
            break;
        }
    }

    close(fd);
    return p.response;
}