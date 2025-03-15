#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "parser/ast.h"
#include "parser/keywords.h"
#include "parser/parser.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"
#include "utils/logger.h"
#include "utils/parser_logger.h"

// DELETE FROM table WHERE search_condition;
ASTNode *parser_row_delete(const Token *tokens) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, KEYWORD_DELETE)) {
    PARSER_LOG_ERROR(tokens[0].position, PARSER_INVALID_KEYWORD,
                     tokens[0].value, KEYWORD_DELETE);
    return NULL;
  }
  if (!expect_token(tokens, 1, TOKEN_KEYWORD, KEYWORD_FROM)) {
    PARSER_LOG_ERROR(tokens[1].position, PARSER_INVALID_KEYWORD,
                     tokens[1].value, KEYWORD_FROM);
    return NULL;
  }
  if (tokens[2].type != TOKEN_IDENTIFIER) {
    PARSER_LOG_ERROR(tokens[2].position, PARSER_INVALID_IDENTIFIER,
                     tokens[2].value, "table_name");
    return NULL;
  }

  ASTNode *node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_DELETE, &node);
  if (ast_result != AST_SUCCESS) {
    DEBUG_LOG("ast", "create", ast_result);
    return NULL;
  }

  node->table_name = strdup(tokens[2].value);
  node->where_condition.column_name = NULL;
  node->where_condition.value = NULL;
  node->where_condition.op = '\0';

  int index = 3;
  if (expect_token(tokens, index, TOKEN_KEYWORD, KEYWORD_WHERE)) {
    index++;
    if (!parser_where_condition(tokens, &index, node)) {
      ast_free(node);
      return NULL;
    }
    node->delete_rows.delete_all = false;
  } else {
    node->delete_rows.delete_all = true;
  }

  return node;
}
