#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../include/ast.h"
#include "../../../include/parser.h"

ASTNode *parser_row_insert(const Token *tokens, int token_count) {
  // INSERT INTO table_name VALUES (value1, value2, value3);
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, "INSERT") ||
      !expect_token(tokens, 1, TOKEN_KEYWORD, "INTO") ||
      tokens[2].type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Syntax error: Expected INSERT INTO <table_name> (...)\n");
    return NULL;
  }

  ASTNode *node = create_ast_node(NODE_INSERT);
  if (!node) {
    return NULL;
  }

  node->table_name = strdup(tokens[2].value);
  if (!node->table_name) {
    perror("Failed to duplicate table name");
    ast_free(node);
    return NULL;
  }

  node->insert_rows.values = NULL;
  node->insert_rows.num_values = 0;

  if (!expect_token(tokens, 3, TOKEN_KEYWORD, "VALUES")) {
    fprintf(stderr, "Syntax error: Expected VALUES at position %d\n",
            tokens[3].position);
    ast_free(node);
    return NULL;
  }

  if (!expect_token(tokens, 4, TOKEN_PUNCTUATION, "(")) {
    fprintf(stderr, "Syntax error: Expected '(' at position %d\n",
            tokens[4].position);
    ast_free(node);
    return NULL;
  }

  int num_values = 0;
  int index = 5;

  // Empty VALUES clause
  if (index < token_count &&
      expect_token(tokens, index, TOKEN_PUNCTUATION, ")")) {
    fprintf(stderr,
            "Syntax error: Expected at least one value inside parentheses\n");
    ast_free(node);
    return NULL;
  }

  while (index < token_count && tokens[index].type == TOKEN_LITERAL) {
    char **temp =
        realloc(node->insert_rows.values, (num_values + 1) * sizeof(char *));
    if (!temp) {
      perror("Failed to reallocate values");
      ast_free(node);
      return NULL;
    }

    node->insert_rows.values = temp;

    node->insert_rows.values[num_values] = strdup(tokens[index].value);
    if (!node->insert_rows.values[num_values]) {
      perror("Failed to duplicate row value name");
      ast_free(node);
      return NULL;
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
      fprintf(stderr, "Syntax error: Expected ',' or ')' but found '%s'\n",
              tokens[index].value);
      ast_free(node);
      return NULL;
    }
  }

  if (index < token_count &&
      !expect_token(tokens, index, TOKEN_PUNCTUATION, ")")) {
    fprintf(stderr, "Syntax error: Expected ')' but found '%s'\n",
            tokens[index].value);
    ast_free(node);
    return NULL;
  }

  return node;
}
