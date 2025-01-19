#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <dictionary.h>
#include <iniparser.h>

#include "reporter_error.h"
#include "sqm_le.h"
#include "wh2600.h"
#include "settings.h"

dictionary * _dict = NULL;
configuration   * _conf = NULL;

void exit_fail(char const * const message, int const exit_code) {
    if(_conf->netxms_log) {
        char command[255] = {0};
        // watch out for "
        snprintf(command, 255, "%s skyreport-fail=true skyreport-fail-message=\"%s\"", _conf->nxapush_bin, message);
        int code;
        if((code = system(command)) != 0) {
            fprintf(stderr, "ERROR: failed to notify nxagent, error code: 0x%X\n", code);;
        }
    }

    fprintf(stderr, "ERROR: %s\n", message);
    exit(exit_code);
}

void handle_sigint(int const sig) {
    fprintf(stderr, "Caught SIGINT, exiting...");
    wh2600_interrupt();
}

void cleanup(void) {
    iniparser_freedict(_dict);
    free(_conf);
}

void log_sample_file(time_t const timestamp, int const humidity, double const temperature, double const brightness) {
    if (!_conf->local_log)
        return;

    struct tm *local_time = {0};
    local_time = localtime (&timestamp);
    char path_date[40] = {0};
    strftime(path_date, 40, "%Y-%m-%d", local_time);

    char path[255] = {0};
    snprintf(path, 255, "%s/skyreport-%s.csv", _conf->log_dir, path_date);
    FILE *output = fopen(path, "a");
    if (!output) {
        char error_msg[255] = {0};
        snprintf(error_msg, 255, "failed to write logs: %s", strerror(errno));
        exit_fail(error_msg, 1);
        return;
    }

    struct stat st;
    stat(path, &st);
    if (st.st_size == 0) {
        fprintf(output, "time;humidity [%%];temperature [°C];surface brightness [mag/arcsec^2];\n"); // print header on new files
    }

    char local_time_s[255] = {0};
    strftime(local_time_s, 255, "%Y-%m-%d %H:%M:%S", local_time);
    fprintf(output, "%s;", local_time_s);

    fprintf(output, "%d;%.2f;%.2f;\n", humidity, temperature, brightness);
    fflush(output);
    fclose(output);
}

void log_sample_netxms(time_t const timestamp, int const humidity, double const temperature, double const brightness) {
    if(!_conf->netxms_log)
        return;

    char command[255] = {0};
    snprintf(command, 255, "%s -t %lu skyreport-fail=false skyreport-surface-brightness=%.2f skyreport-temperature=%.2f skyreport-humidity=%d",
                _conf->nxapush_bin, timestamp, brightness, temperature, humidity);
    
    int code;
    if((code = system(command)) != 0) {
        fprintf(stderr, "ERROR: failed to notify nxagent, error code: 0x%X\n", code);;
    }
}

void sample(void) {
    reporter_error error = {0};

    sqm_le_device device = sqm_le_connect(_conf->sqm_le_addr, _conf->sqm_le_port, &error);
    if (error.kind != REPORTER_NO_ERROR) {
        char error_msg[255] = {0};
        snprintf(error_msg, 255, "failed to connect to sqm_le: %s", error.message);
        exit_fail(error_msg, 1);
    }

    double brightness = sqm_le_read(&device, &error);
    if (error.kind != REPORTER_NO_ERROR) {
        char error_msg[255] = {0};
        snprintf(error_msg, 255, "failed to read from sqm_le: %s", error.message);
        exit_fail(error_msg, 1);
    }

    sqm_le_disconnect(&device);

    wh2600_response weather = wh2600_query(_conf->wh2600_timeout, _conf->wh2600_port, &error);
    if (error.kind != REPORTER_NO_ERROR) {
        char error_msg[255] = {0};
        snprintf(error_msg, 255, "failed to read weather data: %s", error.message);
        exit_fail(error_msg, 1);
    }

    time_t timestamp = time(NULL);

    if(_conf->local_log)
        log_sample_file(timestamp, weather.humidity, weather.temperature, brightness);

    if(_conf->netxms_log)
        log_sample_netxms(timestamp, weather.humidity, weather.temperature, brightness);
}

int main(int argc, char* argv[]) {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGTERM, &sa, NULL);

    iniparser_load("skyreport.ini");

    configuration s = (configuration) {
        .sqm_le_port = (uint16_t) iniparser_getuint64(_dict, ":sqm-le-port", 10001),
        .sqm_le_addr = iniparser_getstring(_dict, ":sqm-le-address", ""),
        .wh2600_port = (uint16_t) iniparser_getuint64(_dict, ":wh2600-port", 8080),
        .log_dir = iniparser_getstring(_dict, ":sqm-le-address", "/var/log/skyreport"),
        .nxapush_bin = iniparser_getstring(_dict, ":sqm-le-address", "nxapush"),
        .sample_count = iniparser_getuint64(_dict, ":sample-count", 3),
        .sample_period = iniparser_getuint64(_dict, ":sample-period", 60 * 60 * 3),
        .wh2600_timeout = iniparser_getuint64(_dict, ":wh2600-timeout", 15),
        .local_log = iniparser_getuint64(_dict, ":enable-log", 1),
        .netxms_log = iniparser_getuint64(_dict, ":enable-netxms", 0),
    };

    _conf = calloc(1, sizeof(s));
    memcpy(_conf, &s, sizeof(s));

    atexit(cleanup);

    for (unsigned i = 0; i < _conf->sample_count; i++) {
        sample();
        sleep(_conf->sample_period);
    }

    return 0;
}    // vlhkost, teplota, cas, svetelnost
