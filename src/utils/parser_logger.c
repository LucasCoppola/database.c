#include <stdio.h>

#include "utils/parser_logger.h"

void parser_error_report(const ParserError *error) {
  const char *error_msg = parser_error_string(error->code);
  fprintf(stderr, "Syntax Error: %s at position %d.\n", error_msg,
          error->position);

  if (error->expected && error->found) {
    fprintf(stderr, "  Found '%s', expected: %s.\n", error->found,
            error->expected);
  }
}

const char *parser_error_string(ParserErrorCode code) {
  switch (code) {
  case PARSER_UNEXPECTED_TOKEN:
    return "Unexpected token";
  case PARSER_INVALID_IDENTIFIER:
    return "Invalid identifier";
  case PARSER_INVALID_KEYWORD:
    return "Invalid keyword";
  case PARSER_INVALID_TYPE:
    return "Invalid column type";
  case PARSER_MISSING_DELIMITER:
    return "Missing delimiter";
  default:
    return "Unknown parser error";
  }
}
