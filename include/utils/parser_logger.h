#ifndef PARSER_LOGGER_H
#define PARSER_LOGGER_H

#include <stdio.h>

typedef enum {
    PARSER_UNEXPECTED_TOKEN,
    PARSER_INVALID_IDENTIFIER,
    PARSER_INVALID_KEYWORD,
    PARSER_INVALID_TYPE,
    PARSER_INVALID_LITERAL,
    PARSER_MISSING_DELIMITER,
    PARSER_UNKNOWN_ERROR
} ParserErrorCode;

typedef struct {
    int position;
    const char *found;
    const char *expected;
    ParserErrorCode code;
} ParserError;

void parser_error_report(const ParserError *error);
const char *parser_error_string(ParserErrorCode code);

#define PARSER_LOG_ERROR(pos, err_code, found_token, expected_token) do { \
    ParserError error = { \
        .position = pos, \
        .code = err_code, \
        .found = found_token, \
        .expected = expected_token \
    }; \
    parser_error_report(&error); \
} while (0)

#endif
