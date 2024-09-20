#include <assert.h>
#include <regex.h>
#include <stdio.h>
#include "sqm_le.h"

int main(void) {
    reporter_error error = {0};

    sqm_le_device device = sqm_le_connect("10.3.1.123", 10001, &error);

    fprintf(stderr, "ERROR: %s\n", error.message);
    assert(error.kind == REPORTER_NO_ERROR);
    
    printf("Brightness: %f\n", sqm_le_read(&device, &error));
    fprintf(stderr, "ERROR: %s\n", error.message);
    assert(error.kind == REPORTER_NO_ERROR);

    sqm_le_disconnect(&device);
}