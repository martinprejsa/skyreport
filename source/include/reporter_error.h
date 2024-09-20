#ifndef REPORTER_ERRORS_H
#define REPORTER_ERRORS_H

typedef enum {
  REPORTER_NO_ERROR = 0,
  REPORTER_CONNECTION_ERROR,
  REPORTER_COMMUNCATION_ERROR,
  REPORTER_ERROR_SIZE
} reporter_error_kind;

typedef struct {
  char* message;
  reporter_error_kind kind;
} reporter_error;

static char const *const reporter_error_strings[] = {"REPORTER_NO_ERROR", "REPORTER_CONNECTION_ERROR", "REPORTER_COMMUNCATION_ERROR"};

#endif // REPORTER_ERRORS_H

