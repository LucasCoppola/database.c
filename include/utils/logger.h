#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#define MAX_ERROR_LENGTH 1024

typedef enum DatabaseResult DatabaseResult;
typedef enum TableResult TableResult;
typedef enum RowResult RowResult;
typedef enum PagerResult PagerResult;
typedef enum HashMapResult HashMapResult;

typedef struct {
    const char* file;
    int line;       
    const char* component;
    const char* operation;
    int code;
} Error;

void error_report(const Error *error);
const char* get_error_message(const char* context, int code);
const char* hashmap_error_string(HashMapResult result);
const char* table_error_string(TableResult result);
const char* database_error_string(DatabaseResult result);
const char* row_error_string(RowResult result);
const char* pager_error_string(PagerResult result);

#define LOG_ERROR(comp, op, err_code) do { \
    Error error = { \
        .file = __FILE__, \
        .line = __LINE__, \
        .component = comp, \
        .operation = op, \
        .code = err_code \
    }; \
    error_report(&error); \
} while(0)

#endif
