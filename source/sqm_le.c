#include "sqm_le.h"

#include <arpa/inet.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h> 
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "reporter_error.h"

char const * find_sqm_le_device_mac(void) {
    return NULL;
}

sqm_le_device sqm_le_connect(char const * const address, uint16_t port, reporter_error *error) {
    int socketfd = 0;

    struct sockaddr_in servaddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(address),
        .sin_port = htons(port),
    };

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        if(error) {
            error->kind = REPORTER_CONNECTION_ERROR;
            error->message = strerror(errno);
        }
        return (sqm_le_device) {0};
    }
    
    if(connect(socketfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0) {
        if(error) {
            error->kind = REPORTER_CONNECTION_ERROR;
            error->message = strerror(errno);
        }

        close(socketfd);
        return (sqm_le_device) {0};
    }
    
    if(error) {
        error->kind = REPORTER_NO_ERROR;
        error->message = "ok";
    }

    return (sqm_le_device) {
        .socket_fd = socketfd,
    };
}

void sqm_le_disconnect(sqm_le_device * const device) {
    close(device->socket_fd);
}

double sqm_le_read(sqm_le_device const * const device, reporter_error *error) {
    char buff[255] = {0};
    
    if (write(device->socket_fd, "rx", 2) != 2) {
        if(error) {
            error->kind = REPORTER_COMMUNCATION_ERROR;
            error->message = strerror(errno);
        }
        return 0;
    }

    if (read(device->socket_fd, buff, 255) <= 0) {
        if(error) {
            error->kind = REPORTER_CONNECTION_ERROR;
            error->message = strerror(errno);
        }
        return 0;
    }

    char brightness_str[5] = {0};
    memcpy(brightness_str, buff+3, 5); // from 2 to 8 = 7 characters, ignoring last and first character

    if(error != NULL) {
        error->kind = REPORTER_NO_ERROR;
        error->message = "ok";
    }
    return atof(brightness_str);
}
