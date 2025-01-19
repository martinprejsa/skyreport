#include "bh1750.h"

bh1750_handle_t* bh1750_init(void);

int
bh1750_set_mode(bh1750_handle_t * const handle, bh1750_mode_t const mode);

int
bh1750_set_measurement_time(bh1750_handle_t * const handle);

int32_t 
bh1750_read_measurement(bh1750_handle_t * const handle);

void
bh1750_destroy(bh1750_handle_t * handle);