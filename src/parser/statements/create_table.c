#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

ASTNode *parser_table_create(const Token *tokens, int token_count) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, "CREATE") ||
      !expect_token(tokens, 1, TOKEN_KEYWORD, "TABLE") ||
      tokens[2].type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Syntax error: Expected CREATE TABLE <name> (...)\n");
    return NULL;
  }

  ASTNode *node = create_ast_node(NODE_CREATE_TABLE);
  if (!node) {
    return NULL;
  }

  node->table_name = strdup(tokens[2].value);
  if (!node->table_name) {
    perror("Failed to duplicate table name");
    ast_free(node);
    return NULL;
  }

  node->create_table.columns = NULL;

  if (!expect_token(tokens, 3, TOKEN_PUNCTUATION, "(")) {
    fprintf(stderr, "Syntax error: Expected '(' at position %d\n",
            tokens[3].position);
    ast_free(node);
    return NULL;
  }

  int index = 4;
  int num_columns = 0;
  Column *columns = columns_parse(tokens, token_count, &index, &num_columns);
  if (!columns) {
    ast_free(node);
    return NULL;
  }

  node->create_table.columns = columns;
  node->create_table.num_columns = num_columns;

  return node;
}

Column *columns_parse(const Token *tokens, int token_count, int *index,
                      int *num_columns) {
  Column *columns = malloc(sizeof(Column) * MAX_COLUMNS);
  if (!columns) {
    fprintf(stderr, "Failed to allocate columns\n");
    return NULL;
  }

  *num_columns = 0;
  while (*index < token_count) {
    if (tokens[*index].type != TOKEN_IDENTIFIER) {
      fprintf(stderr, "Syntax error: Expected column name at position %d\n",
              tokens[*index].position);
      columns_free(columns, *num_columns);
      return NULL;
    }

    columns[*num_columns].name = strdup(tokens[*index].value);
    if (!columns[*num_columns].name) {
      perror("Failed to duplicate column name");
      columns_free(columns, *num_columns);
      return NULL;
    }
    (*index)++;

    if (*index >= token_count || tokens[*index].type != TOKEN_KEYWORD) {
      fprintf(stderr, "Syntax error: Expected column type after '%s'\n",
              columns[*num_columns].name);
      free(columns[*num_columns].name);
      columns_free(columns, *num_columns);
      return NULL;
    }

    columns[*num_columns].type = map_column_type(tokens[*index].value);
    (*index)++;
    (*num_columns)++;

    if (*index >= token_count || tokens[*index].type != TOKEN_PUNCTUATION) {
      fprintf(stderr,
              "Syntax error: Expected ',' or ')' after column definition\n");
      columns_free(columns, *num_columns);
      return NULL;
    }

    if (strcmp(tokens[*index].value, ",") == 0) {
      (*index)++;
    } else if (strcmp(tokens[*index].value, ")") == 0) {
      (*index)++;
      break;
    } else {
      fprintf(stderr, "Syntax error: Expected ',' or ')' but found '%s'\n",
              tokens[*index].value);
      columns_free(columns, *num_columns);
      return NULL;
    }
  }
  return columns;
}

// This frees the local columns array not the one in the ASTNode
void columns_free(Column *columns, int num_columns) {
  if (!columns)
    return;

  for (int i = 0; i < num_columns; i++) {
    if (columns[i].name) {
      free(columns[i].name);
    }
  }
  free(columns);
}
