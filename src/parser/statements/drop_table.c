#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"
#include "utils/parser_logger.h"

ASTNode *parser_table_drop(const Token *tokens) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, "DROP")) {
    PARSER_LOG_ERROR(tokens[0].position, PARSER_INVALID_KEYWORD,
                     tokens[0].value, "DROP");
    return NULL;
  }
  if (!expect_token(tokens, 1, TOKEN_KEYWORD, "TABLE")) {
    PARSER_LOG_ERROR(tokens[1].position, PARSER_INVALID_KEYWORD,
                     tokens[1].value, "TABLE");
    return NULL;
  }
  if (tokens[2].type != TOKEN_IDENTIFIER) {
    PARSER_LOG_ERROR(tokens[2].position, PARSER_INVALID_IDENTIFIER,
                     tokens[2].value, "table_name");
    return NULL;
  }

  ASTNode *node = create_ast_node(NODE_DROP_TABLE);
  if (!node) {
    printf("Failed to create ast node\n");
    return NULL;
  }

  node->table_name = strdup(tokens[2].value);
  if (!node->table_name) {
    printf("Failed to duplicate table name");
    ast_free(node);
    return NULL;
  }

  return node;
}
