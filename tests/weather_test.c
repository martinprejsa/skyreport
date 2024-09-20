#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "reporter_error.h"
#include "wh2600.h"


int main() {
    reporter_error error = {0};
    wh2600_response resp = wh2600_query(10, 8080, &error);

    if (error.kind != REPORTER_NO_ERROR) {
        fprintf(stderr, "ERROR: %s\n", error.message); 
        exit(1);
    }

    printf("Humidity: %d\n", resp.humidity);
    printf("Temperature: %f\n", resp.temperature);
}