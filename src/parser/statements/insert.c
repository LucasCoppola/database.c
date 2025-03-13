#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"
#include "parser/parser.h"
#include "utils/convertions.h"
#include "utils/logger.h"
#include "utils/parser_logger.h"

#include "core/row.h"

// INSERT INTO table_name VALUES (value1, value2, value3);
ASTNode *parser_row_insert(const Token *tokens, int token_count) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, "INSERT")) {
    PARSER_LOG_ERROR(tokens[0].position, PARSER_INVALID_KEYWORD,
                     tokens[0].value, "INSERT");
    return NULL;
  }
  if (!expect_token(tokens, 1, TOKEN_KEYWORD, "INTO")) {
    PARSER_LOG_ERROR(tokens[1].position, PARSER_INVALID_KEYWORD,
                     tokens[1].value, "INTO");
    return NULL;
  }
  if (tokens[2].type != TOKEN_IDENTIFIER) {
    PARSER_LOG_ERROR(tokens[2].position, PARSER_INVALID_IDENTIFIER,
                     tokens[2].value, "table_name");
    return NULL;
  }

  ASTNode *node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_INSERT, &node);
  if (ast_result != AST_SUCCESS) {
    DEBUG_LOG("ast", "create", ast_result);
    return NULL;
  }

  node->table_name = strdup(tokens[2].value);
  if (!node->table_name) {
    fprintf(stderr, "Failed to duplicate table name");
    ast_free(node);
    return NULL;
  }

  node->insert_rows.values = NULL;
  node->insert_rows.num_values = 0;

  if (!expect_token(tokens, 3, TOKEN_KEYWORD, "VALUES")) {
    PARSER_LOG_ERROR(tokens[3].position, PARSER_INVALID_KEYWORD,
                     tokens[3].value, "VALUES");
    ast_free(node);
    return NULL;
  }

  if (!expect_token(tokens, 4, TOKEN_PUNCTUATION, "(")) {
    PARSER_LOG_ERROR(tokens[4].position, PARSER_MISSING_DELIMITER,
                     tokens[4].value, "(");
    ast_free(node);
    return NULL;
  }

  int num_values = 0;
  int index = 5;

  while (index < token_count) {
    if (tokens[index].type != TOKEN_STRING_LITERAL &&
        tokens[index].type != TOKEN_INTEGER_LITERAL &&
        tokens[index].type != TOKEN_REAL_LITERAL &&
        tokens[index].type != TOKEN_BOOLEAN_LITERAL) {
      PARSER_LOG_ERROR(tokens[index].position, PARSER_INVALID_LITERAL,
                       tokens[index].value, "column_value");
      ast_free(node);
      return NULL;
    }

    Value **temp =
        realloc(node->insert_rows.values, (num_values + 1) * sizeof(Value));
    if (!temp) {
      fprintf(stderr, "Failed to reallocate values");
      ast_free(node);
      return NULL;
    }

    node->insert_rows.values = temp;

    if (tokens[index].type == TOKEN_STRING_LITERAL) {
      Value *value = convert_value(tokens[index].value, COLUMN_TYPE_TEXT);
      node->insert_rows.values[num_values] = value;
    } else if (tokens[index].type == TOKEN_INTEGER_LITERAL) {
      Value *value = convert_value(tokens[index].value, COLUMN_TYPE_INT);
      node->insert_rows.values[num_values] = value;
    } else if (tokens[index].type == TOKEN_REAL_LITERAL) {
      Value *value = convert_value(tokens[index].value, COLUMN_TYPE_REAL);
      node->insert_rows.values[num_values] = value;
    } else if (tokens[index].type == TOKEN_BOOLEAN_LITERAL) {
      Value *value = convert_value(tokens[index].value, COLUMN_TYPE_BOOL);
      node->insert_rows.values[num_values] = value;
    }

    num_values++;
    // update after each loop so if it fails, it can free the values
    node->insert_rows.num_values = num_values;
    index++;

    if (expect_token(tokens, index, TOKEN_PUNCTUATION, ",")) {
      index++;
    } else if (expect_token(tokens, index, TOKEN_PUNCTUATION, ")")) {
      break;
    } else {
      PARSER_LOG_ERROR(tokens[index].position, PARSER_MISSING_DELIMITER,
                       tokens[index].value, "',' or ')'");
      ast_free(node);
      return NULL;
    }
  }

  if (index < token_count &&
      !expect_token(tokens, index, TOKEN_PUNCTUATION, ")")) {
    PARSER_LOG_ERROR(tokens[index].position, PARSER_MISSING_DELIMITER,
                     tokens[index].value, ")");
    ast_free(node);
    return NULL;
  }

  return node;
}
