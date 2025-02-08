#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../include/parser.h"
#include "../../../include/tokenizer.h"

ASTNode *parser_table_drop(const Token *tokens) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, "DROP") ||
      !expect_token(tokens, 1, TOKEN_KEYWORD, "TABLE") ||
      tokens[2].type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Syntax error: Expected DROP TABLE <name>\n");
    return NULL;
  }

  ASTNode *node = create_ast_node(NODE_DROP_TABLE);
  if (!node) {
    return NULL;
  }
  node->table_name = tokens[2].value;
  return node;
}
