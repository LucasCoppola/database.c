#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../include/ast.h"
#include "../../../include/parser.h"
#include "../../../include/tokenizer.h"

ASTNode *parser_row_select(const Token *tokens, int token_count) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, "SELECT")) {
    fprintf(stderr,
            "Syntax error: Expected SELECT at the start of the query\n");
    return NULL;
  }

  ASTNode *node = create_ast_node(NODE_SELECT);
  if (!node) {
    return NULL;
  }

  node->select_rows.select_columns = NULL;
  node->table_name = NULL;

  bool select_all = false;
  int num_columns = 0;
  int index = 1;

  if (expect_token(tokens, index, TOKEN_WILDCARD, "*")) {
    select_all = true;
    index++;
  } else if (index < token_count && tokens[index].type == TOKEN_IDENTIFIER) {
    node->select_rows.select_columns = malloc(sizeof(char *));
    if (!node->select_rows.select_columns) {
      perror("Failed to allocate first col in ASTNode\n");
      ast_free(node);
      return NULL;
    }

    while (index < token_count && tokens[index].type == TOKEN_IDENTIFIER) {
      char **temp = realloc(node->select_rows.select_columns,
                            (num_columns + 1) * sizeof(char *));
      if (!temp) {
        perror("Failed to reallocate columns");
        ast_free(node);
        return NULL;
      }

      node->select_rows.select_columns = temp;

      node->select_rows.select_columns[num_columns] =
          strdup(tokens[index].value);
      if (!node->select_rows.select_columns[num_columns]) {
        perror("Failed to duplicate column name");
        ast_free(node);
        return NULL;
      }

      num_columns++;
      // update after each loop so if it fails, it can free the cols
      node->select_rows.num_columns = num_columns;
      index++;

      if (index < token_count &&
          expect_token(tokens, index, TOKEN_PUNCTUATION, ",")) {
        index++;
      } else {
        break;
      }
    }
  } else {
    fprintf(stderr, "Syntax error: Expected column list after SELECT\n");
    ast_free(node);
    return NULL;
  }

  if (index >= token_count ||
      !expect_token(tokens, index, TOKEN_KEYWORD, "FROM")) {
    fprintf(stderr, "Syntax error: Expected FROM after column list\n");
    ast_free(node);
    return NULL;
  }
  index++;

  if (index >= token_count || tokens[index].type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Syntax error: Expected table name after FROM\n");
    ast_free(node);
    return NULL;
  }

  node->table_name = strdup(tokens[index].value);
  if (!node->table_name) {
    perror("Failed to duplicate table name");
    ast_free(node);
    return NULL;
  }
  node->select_rows.num_columns = num_columns;
  node->select_rows.select_all = select_all;
  return node;
}
