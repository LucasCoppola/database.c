#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/statements.h"
#include "parser/tokenizer.h"
#include "utils/parser_logger.h"

ASTNode *parse(const Token *tokens, int token_count) {
  if (token_count == 0)
    return NULL;

  if (tokens[0].type != TOKEN_KEYWORD) {
    PARSER_LOG_ERROR(tokens[0].position, PARSER_UNEXPECTED_TOKEN, NULL, NULL);
    return NULL;
  }

  char *value = tokens[0].value;

  if (strcmp(value, "CREATE") == 0) {
    return parser_table_create(tokens, token_count);
  } else if (strcmp(value, "DROP") == 0) {
    return parser_table_drop(tokens);
  } else if (strcmp(value, "SELECT") == 0) {
    return parser_row_select(tokens, token_count);
  } else if (strcmp(value, "INSERT") == 0) {
    return parser_row_insert(tokens, token_count);
  } else {
    PARSER_LOG_ERROR(tokens[0].position, PARSER_INVALID_KEYWORD, NULL, NULL);
    return NULL;
  }
}
