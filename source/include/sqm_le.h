#ifndef REPORTER_SQM_LE_H
#define REPORTER_SQM_LE_H

#include <stdint.h>

#include "reporter_error.h"

typedef struct {
    int socket_fd;
} sqm_le_device;

/**
* Connects to the specified device via TCP/IP
* Returns the device handle, or sets the error if apropriate
* */
sqm_le_device sqm_le_connect(char const * const address, uint16_t port, reporter_error *error);

/**
* Disconnects the device cleanly
* */
void sqm_le_disconnect(sqm_le_device * const);

/**
* Performs average read on the device
* Returns the read value, or sets the error if approriate
* */
double sqm_le_read(sqm_le_device const * const device, reporter_error *error);

#endif // REPORTER_SQM_LE_H

