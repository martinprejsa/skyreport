#ifndef SKYREPORT_CONFIGURATION_H
#define SKYREPORT_CONFIGURATION_H

#include <stdint.h>

typedef struct {
    uint16_t const sqm_le_port;
    char const * const sqm_le_addr;

    uint16_t const wh2600_port;
    uint64_t const wh2600_timeout;

    char const * const log_dir;
    char const * const nxapush_bin;

    uint64_t const sample_count;
    uint64_t const sample_period;

    uint64_t const local_log;
    uint64_t const netxms_log;
} configuration;

#endif // SKYREPORT_CONFIGURATION_H
