#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#include "database.h"
#include "hashmap.h"
#include "storage.h"

typedef struct {
    const char* message;
    int code;
    const char* context;
} ErrorInfo;

void log_error(const ErrorInfo* error);
const char* get_error_message(const char* context, int code);

const char* hashmap_error_string(HashMapResult result);
const char* table_error_string(TableResult result);
const char* database_error_string(DatabaseResult result);
const char *row_error_string(RowResult result);
const char *pager_error_string(PagerResult result);

// LOG_ERROR: It should be used one level above implementation
#define LOG_ERROR(ctx, err_code) do { \
    ErrorInfo error = { \
        .message = get_error_message(ctx, err_code), \
        .code = err_code, \
        .context = ctx \
    }; \
    log_error(&error); \
} while(0)

#endif
