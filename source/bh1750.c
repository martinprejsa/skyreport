#include "bh1750.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>

// calculates difference in miliseconds between two timespec structs
uint64_t 
timespec_mili_diff(struct timespec const * const t1, struct timespec const * const  t2) {
	uint64_t diff = 0;
	diff += (t1->tv_sec - t2->tv_sec) * 1000;
	uint64_t diff_nsec = (t1->tv_nsec - t2->tv_nsec);
	if (diff_nsec > 1e6) {
	    double diff_f = diff_nsec / 1e6;
	    uint64_t diff_i = floor(diff_f);
	    diff += diff_i;
	}

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

    fd = open(filename, O_RDWR);
    if (fd < 0) {
		error_callback("open failed: %s", strerror(errno));
  		return 0;
	}

	int addr = 0x23; /* TODO: config */

	if(ioctl(fd, I2C_SLAVE, addr) < 0) {
		error_callback("failed to set address: %s", strerror(errno));
		return 0;
	}

	handle->fd = fd;
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

	if (write(handle->fd, (uint8_t*) &mode, 1) != 1) {
		error_callback("failed to set mode: %s\n", strerror(errno));
		return 0;
	}
	
	timespec_get(&handle->since, TIME_UTC);
	handle->mode = mode;
}

int
bh1750_set_measurement_time(bh1750_handle_t * const handle);

int32_t 
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

	uint64_t diff = timespec_mili_diff(&now, &handle->since);
	printf("%" PRIu64 "\n", diff);
}

void
bh1750_destroy(bh1750_handle_t * handle);
