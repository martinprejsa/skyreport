#ifndef REPORTER_WEATHER_H
#define REPORTER_WEATHER_H

#include <netinet/in.h>
#include <sys/socket.h>

#include "reporter_error.h"

typedef struct {
    double temperature;
    int humidity;
} wh2600_response;

/** 
* Starts a minimal HTTP server on `port` and awaits a GET request from the WH2600 station.
* Once this request is recieved, the function finishes returning the recieved data.
* If no request is recieved within `timeout` seconds,
* the function finishes returning empty response, and setting the approriate error.
*/
wh2600_response wh2600_query(uint64_t timeout, uint16_t port, reporter_error * error);

#endif // REPORTER_WEATHER_H
