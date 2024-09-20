#include <stdint.h>

typedef struct {
    uint16_t sqm_le_port;
    char const * sqm_le_addr;

    uint16_t wh2600_port;
    uint64_t wh2600_timeout;

    char const * log_dir;
    char const * nxapush_bin;

    uint64_t sample_count;
    uint64_t sample_period;

    uint64_t local_log;
    uint64_t netxms_log;
} settings;