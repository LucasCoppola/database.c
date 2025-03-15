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

// SELECT column_name/* FROM table WHERE search_condition;
ASTNode *parser_row_select(const Token *tokens, int token_count) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, KEYWORD_SELECT)) {
    PARSER_LOG_ERROR(tokens[0].position, PARSER_INVALID_KEYWORD,
                     tokens[0].value, KEYWORD_SELECT);
    return NULL;
  }

  ASTNode *node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_SELECT, &node);
  if (ast_result != AST_SUCCESS) {
    DEBUG_LOG("ast", "create", ast_result);
    return NULL;
  }

  node->table_name = NULL;

  node->select_rows.select_columns = NULL;
  node->select_rows.num_columns = 0;
  node->select_rows.select_all = false;

  node->where_condition.column_name = NULL;
  node->where_condition.value = NULL;
  node->where_condition.op = '\0';

  int index = 1;
  bool select_all = expect_token(tokens, index, TOKEN_WILDCARD, "*");

  if (select_all) {
    index++;
  } else if (index < token_count && tokens[index].type == TOKEN_IDENTIFIER) {
    while (index < token_count && tokens[index].type == TOKEN_IDENTIFIER) {
      char **temp =
          realloc(node->select_rows.select_columns,
                  (node->select_rows.num_columns + 1) * sizeof(char *));
      if (!temp) {
        fprintf(stderr, "Failed to reallocate columns");
        ast_free(node);
        return NULL;
      }

      node->select_rows.select_columns = temp;
      node->select_rows.select_columns[node->select_rows.num_columns] =
          strdup(tokens[index].value);
      if (!node->select_rows.select_columns[node->select_rows.num_columns]) {
        fprintf(stderr, "Failed to duplicate column name");
        ast_free(node);
        return NULL;
      }

      node->select_rows.num_columns++;
      index++;

      if (index < token_count &&
          expect_token(tokens, index, TOKEN_PUNCTUATION, PUNCT_COMMA)) {
        index++;
      } else {
        break;
      }
    }
  } else {
    PARSER_LOG_ERROR(tokens[index].position, PARSER_INVALID_IDENTIFIER,
                     tokens[index].value, "column_name or *");
    ast_free(node);
    return NULL;
  }

  if (!expect_token(tokens, index, TOKEN_KEYWORD, KEYWORD_FROM)) {
    PARSER_LOG_ERROR(tokens[index].position, PARSER_INVALID_KEYWORD,
                     tokens[index].value, KEYWORD_FROM);
    ast_free(node);
    return NULL;
  }
  index++;

  if (index >= token_count || tokens[index].type != TOKEN_IDENTIFIER) {
    PARSER_LOG_ERROR(tokens[index].position, PARSER_INVALID_IDENTIFIER,
                     tokens[index].value, "table_name");
    ast_free(node);
    return NULL;
  }

  node->table_name = strdup(tokens[index].value);
  if (!node->table_name) {
    fprintf(stderr, "Failed to duplicate table name");
    ast_free(node);
    return NULL;
  }
  index++;

  if (expect_token(tokens, index, TOKEN_KEYWORD, KEYWORD_WHERE)) {
    index++;
    if (!parser_where_condition(tokens, &index, node)) {
      ast_free(node);
      return NULL;
    }
  }

  node->select_rows.select_all = select_all;
  return node;
}
