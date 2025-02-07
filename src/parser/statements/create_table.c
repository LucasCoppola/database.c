#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../include/parser.h"
#include "../../../include/tokenizer.h"

ASTNode *parser_table_create(const Token *tokens, int token_count) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, "CREATE") ||
      !expect_token(tokens, 1, TOKEN_KEYWORD, "TABLE") ||
      tokens[2].type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Syntax error: Expected CREATE TABLE <name> (...)\n");
    return NULL;
  }

  ASTNode *node = create_ast_node(NODE_CREATE_TABLE);
  node->table_name = tokens[2].value;

  if (!expect_token(tokens, 3, TOKEN_PUNCTUATION, "(")) {
    fprintf(stderr, "Syntax error: Expected '(' at position %d\n",
            tokens[3].position);
    free(node);
    return NULL;
  }

  int index = 4;
  int num_columns = 0;
  Column *columns = parse_columns(tokens, token_count, &index, &num_columns);
  if (!columns) {
    free(node);
    return NULL;
  }

  node->create_table.columns = columns;
  node->create_table.num_columns = num_columns;

  return node;
}

Column *parse_columns(const Token *tokens, int token_count, int *index,
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
      free(columns);
      return NULL;
    }

    char *col_name = tokens[*index].value;
    (*index)++;

    if (*index >= token_count || tokens[*index].type != TOKEN_KEYWORD) {
      fprintf(stderr, "Syntax error: Expected column type after '%s'\n",
              col_name);
      free(columns);
      return NULL;
    }

    char *col_type = tokens[*index].value;
    (*index)++;

    columns[*num_columns].name = col_name;
    columns[*num_columns].type = map_column_type(col_type);
    (*num_columns)++;

    if (*index >= token_count || tokens[*index].type != TOKEN_PUNCTUATION) {
      fprintf(stderr,
              "Syntax error: Expected ',' or ')' after column definition\n");
      free(columns);
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
      free(columns);
      return NULL;
    }
  }

  return columns;
}
