#include "bh1750.h"

int main() {
	bh1750_handle_t handle;
	bh1750_init(&handle);
	bh1750_set_mode(&handle, BH1750_MODE_CONT_HIGH_RES);
	bh1750_read_measurement(&handle);
}
