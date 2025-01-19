#ifndef SKYREPORT_BH1750_H
#define SKYREPORT_BH1750_H

#include <stdint.h>

typedef enum bh1750_cmd {
    BH1750_CMD_POWER_DOWN = 0x0,
    BH1750_CMD_POWER_ON   = 0x1,
    BH1750_CMD_POWER_RESE = 0x7,
} bh1750_cmd_t;

typedef enum bh1750_mode {
    BH1750_MODE_UNKNOWN = 0,

    BH1750_MODE_CONT_HIGH_RES  = 0x10,
    BH1750_MODE_CONT_HIGH_RES2 = 0x11,
    BH1750_CONT_LOW_RES   = 0x13,
    
    BH1750_MODE_ONE_TIME_HIGH_RES  = 0x20,
    BH1750_MODE_ONE_TIME_HIGH_RES2 = 0x21,
    BH1750_MODE_ONE_TIME_LOW_RES   = 0x23,
} bh1750_mode_t;

typedef struct bh1750_handle {
    bh1750_mode_t mode;
} bh1750_handle_t;

/*
* Locate the BH1750 device on the i2c bus.
* Return 1 on success, 0 otherwise.
*/
int bh1750_init(bh1750_handle_t * handle);

/*
* Set the error callback
* If callback is NULL, the default callback is used.
*/
void 
bh1750_set_error_callback(int (*callback)(char const * const format, ...));

/*
* Set the BH1750 device mode.
* Return 1 on success, 0 otherwise.1
*/
int
bh1750_set_mode(bh1750_handle_t * const handle, bh1750_mode_t const mode);

/*
* Set the BH1750 device measurement time.
* Return 1 on success, 0 otherwise.
*/
int
bh1750_set_measurement_time(bh1750_handle_t * const handle);

/*
* Read a measurement from the BH1750 device.
* Return the measured value in lux, -1 on error. 
*
* The correct timing will be applied acording to 
* the currently selected operating mode and the function will block.
*/
int32_t 
bh1750_read_measurement(bh1750_handle_t * const handle);

/*
* Destroy the handle
*/
void
bh1750_destroy(bh1750_handle_t * handle);

#endif // SKYREPORT_BH1750_H
