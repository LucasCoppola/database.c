#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"

bool expect_token(const Token *tokens, int index, TokenType type,
                  const char *value) {
  return tokens[index].type == type && strcmp(tokens[index].value, value) == 0;
}

DataType map_column_type(char *type) {
  if (strcmp(type, "INT") == 0 || strcmp(type, "INTEGER") == 0) {
    return TYPE_INT;
  } else if (strcmp(type, "TEXT") == 0) {
    return TYPE_TEXT;
  } else {
    printf("Unkown column type %s\n", type);
    return TYPE_UNKNOWN;
  }
}
