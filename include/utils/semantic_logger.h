#ifndef SEMANTIC_LOGGER_H
#define SEMANTIC_LOGGER_H

#include <stdio.h>

typedef SemanticResult SemanticResult;

typedef struct {
    SemanticResult code; 
    const char* found;     
    const char* expected; 
} SemanticError;

void semantic_error_report(const SemanticError* error);

#define SEMANTIC_LOG_ERROR(err_code, found_token, expected_token) \
    do { \
        SemanticError error = { \
            .code = err_code, \
            .found = found_token, \
            .expected = expected_token \
        }; \
        semantic_error_report(&error); \
    } while (0)

#endif
