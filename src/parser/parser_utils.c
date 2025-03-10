#include <stdbool.h>
#include <string.h>

#include "parser/parser.h"
#include "parser/tokenizer.h"

bool expect_token(const Token *tokens, int index, TokenType type,
                  const char *value) {
  return tokens[index].type == type && strcmp(tokens[index].value, value) == 0;
}
