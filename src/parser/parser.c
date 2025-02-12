#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/parser/statements.h"
#include "../../include/parser/tokenizer.h"

void *parse(const Token *tokens, int token_count) {
  if (token_count == 0)
    return NULL;

  if (tokens[0].type != TOKEN_KEYWORD) {
    printf("Syntax Error: Expected keyword at start of statement\n");
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
    fprintf(stderr, "Syntax Error: Unknown statement '%s'\n", value);
    return NULL;
  }
}
