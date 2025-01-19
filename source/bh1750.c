#include "bh1750.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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
}

void 
bh1750_set_error_callback(int (*cb)(const char *, ...)) {
    if(cb == NULL)
        error_callback = default_error_callback;
    else
        error_callback = cb;
}

int
bh1750_set_mode(bh1750_handle_t * const handle, bh1750_mode_t const mode);

int
bh1750_set_measurement_time(bh1750_handle_t * const handle);

int32_t 
bh1750_read_measurement(bh1750_handle_t * const handle);

void
bh1750_destroy(bh1750_handle_t * handle);