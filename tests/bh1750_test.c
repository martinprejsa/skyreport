#include "bh1750.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
	bh1750_handle_t handle;
	
	if(!bh1750_init(&handle))
	    exit(EXIT_FAILURE);
	
	if(!bh1750_set_mode(&handle, BH1750_MODE_ONE_TIME_HIGH_RES))
	    exit(EXIT_FAILURE);

	float m = bh1750_read_measurement(&handle);
	if(m == -1)
	    exit(EXIT_FAILURE);
	printf("%.2f lux\n", m);
}
