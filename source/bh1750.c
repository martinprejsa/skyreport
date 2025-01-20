#include "bh1750.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <threads.h>
#include <math.h>
#include <stdlib.h>

uint64_t mode_max_timeout(bh1750_mode_t mode) {
    switch(mode) {
        case BH1750_MODE_UNKNOWN:
            return 0;
        case BH1750_MODE_CONT_HIGH_RES:
            return 180;
        case BH1750_MODE_CONT_HIGH_RES2:
            return 180;
        case BH1750_CONT_LOW_RES:
            return 24;
        case BH1750_MODE_ONE_TIME_HIGH_RES:
            return 180;
        case BH1750_MODE_ONE_TIME_HIGH_RES2:
            return 180;
        case BH1750_MODE_ONE_TIME_LOW_RES:
            return 24;
    }

    return 0;
}
// calculates difference in nanoseconds between two timespec structs
uint64_t 
timespec_nano_diff(struct timespec const * const t1, struct timespec const * const  t2) {
	uint64_t diff = 0;
	diff += (t1->tv_sec - t2->tv_sec) * 1e9;
	diff += (t1->tv_nsec - t2->tv_nsec);
	return diff;
}

/*
* Default error callback, wraps fprintf to stderr
*/
static int default_error_callback(char const * const format, ...)
{
  int ret;
  va_list argptr;
  va_start(argptr, format);
  ret = vfprintf(stderr, format, argptr);
  va_end(argptr);
  return ret;
}

static int (*error_callback)(char const * const format, ...) = default_error_callback;

int bh1750_init(bh1750_handle_t * handle) {
    memset(handle, 0, sizeof(bh1750_handle_t)); 
    handle->mode = BH1750_MODE_UNKNOWN;
    
    int fd;
    int adapter_nr = 1; /* TODO: should be dynamically determined */
    char filename[20] = {0};
    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);

    // open the i2c device bus
    fd = open(filename, O_RDWR);
    if (fd < 0) {
		error_callback("open failed: %s", strerror(errno));
  		return 0;
	}

    // 0x23 is the default address
	int addr = 0x23; /* TODO: config */

    // setting the slave address
	if(ioctl(fd, I2C_SLAVE, addr) < 0) {
		error_callback("failed to set address: %s", strerror(errno));
		return 0;
	}

	handle->fd = fd;
    return 1;
}

void 
bh1750_set_error_callback(int (*cb)(const char *, ...)) {
    if(cb == NULL)
        error_callback = default_error_callback;
    else
        error_callback = cb;
}

int
bh1750_set_mode(bh1750_handle_t * const handle, bh1750_mode_t const mode) {
	if(handle == NULL) {
		error_callback("handle can not be NULL\n");
		return 0;
	}
    // write the mode enum value
	if (write(handle->fd, (uint8_t*) &mode, 1) != 1) {
		error_callback("failed to set mode: %s\n", strerror(errno));
		return 0;
	}
	
	timespec_get(&handle->since, TIME_UTC);
	handle->mode = mode;
    return 1;
}

int
bh1750_set_measurement_time(bh1750_handle_t * const handle);

float 
bh1750_read_measurement(bh1750_handle_t * const handle) {
	if(handle == NULL) {
		error_callback("handle can not be NULL\n");
		return 0;
	}

	if(handle->mode == BH1750_MODE_UNKNOWN) {
		error_callback("the mode is not set\n");
		return 0;
	}

	struct timespec now;
	timespec_get(&now, TIME_UTC);

    // Calculate the difference from now to the last command execution.
    // Then subtract it from the maximum timeout.
    // The result is the amount of time that we should wait.
	uint64_t diff = mode_max_timeout(handle->mode) - timespec_nano_diff(&now, &handle->since);
    if (diff > 0)
        thrd_sleep(&(struct timespec) {.tv_nsec=diff}, NULL);

    uint8_t buffer[2] = {0};
    if(read(handle->fd, buffer, sizeof(buffer)) != sizeof(buffer)) {
		error_callback("failed to read measurement result: %s\n", strerror(errno));
		return 0;
    }

    int16_t result = 0;
    result += (uint16_t) buffer[1]; 
    result += (uint16_t) (buffer[0] << 8);

    float result_fixed = result;
    result_fixed /= 1.2; // magic number from bh1750 documentation

    return result_fixed;
}

void
bh1750_destroy(bh1750_handle_t * handle) {
    close(handle->fd);
}
