#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"
#include "utils/convertions.h"
#include "utils/parser_logger.h"

bool parser_where_condition(const Token *tokens, int *index, ASTNode *node) {
  if (tokens[*index].type != TOKEN_IDENTIFIER) {
    PARSER_LOG_ERROR(tokens[*index].position, PARSER_INVALID_IDENTIFIER,
                     tokens[*index].value, "column_name");
    return false;
  }

  node->where_condition.column_name = strdup(tokens[*index].value);
  (*index)++;

  if (tokens[*index].type != TOKEN_OPERATOR) {
    PARSER_LOG_ERROR(tokens[*index].position, PARSER_INVALID_OPERATOR,
                     tokens[*index].value, "<, >, =");
    return false;
  }

  char *op_str = strdup(tokens[*index].value);
  if (op_str != NULL && op_str[0] != '\0') {
    node->where_condition.op = op_str[0];
  }
  free(op_str);
  (*index)++;

  if (tokens[*index].type != TOKEN_STRING_LITERAL &&
      tokens[*index].type != TOKEN_INTEGER_LITERAL &&
      tokens[*index].type != TOKEN_REAL_LITERAL &&
      tokens[*index].type != TOKEN_BOOLEAN_LITERAL) {
    PARSER_LOG_ERROR(tokens[*index].position, PARSER_INVALID_LITERAL,
                     tokens[*index].value, "literal_value");
    return false;
  }

  if (tokens[*index].type == TOKEN_STRING_LITERAL) {
    Value *value = convert_value(tokens[*index].value, COLUMN_TYPE_TEXT);
    node->where_condition.value = value;
  } else if (tokens[*index].type == TOKEN_INTEGER_LITERAL) {
    Value *value = convert_value(tokens[*index].value, COLUMN_TYPE_INT);
    node->where_condition.value = value;
  } else if (tokens[*index].type == TOKEN_REAL_LITERAL) {
    Value *value = convert_value(tokens[*index].value, COLUMN_TYPE_REAL);
    node->where_condition.value = value;
  } else if (tokens[*index].type == TOKEN_BOOLEAN_LITERAL) {
    Value *value = convert_value(tokens[*index].value, COLUMN_TYPE_BOOL);
    node->where_condition.value = value;
  }
  (*index)++;

  return true;
}
