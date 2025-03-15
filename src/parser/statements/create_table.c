#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"
#include "parser/keywords.h"
#include "parser/parser.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"
#include "utils/logger.h"
#include "utils/parser_logger.h"

ASTNode *parser_table_create(const Token *tokens, int token_count) {
  if (!expect_token(tokens, 0, TOKEN_KEYWORD, KEYWORD_CREATE)) {
    PARSER_LOG_ERROR(tokens[0].position, PARSER_INVALID_KEYWORD,
                     tokens[0].value, KEYWORD_CREATE);
    return NULL;
  }
  if (!expect_token(tokens, 1, TOKEN_KEYWORD, KEYWORD_TABLE)) {
    PARSER_LOG_ERROR(tokens[1].position, PARSER_INVALID_KEYWORD,
                     tokens[1].value, KEYWORD_TABLE);
    return NULL;
  }
  if (tokens[2].type != TOKEN_IDENTIFIER) {
    PARSER_LOG_ERROR(tokens[2].position, PARSER_INVALID_IDENTIFIER,
                     tokens[2].value, "table_name");
    return NULL;
  }

  ASTNode *node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_CREATE_TABLE, &node);
  if (ast_result != AST_SUCCESS) {
    DEBUG_LOG("ast", "create", ast_result);
    return NULL;
  }

  node->table_name = strdup(tokens[2].value);
  if (!node->table_name) {
    printf("Failed to duplicate table name");
    ast_free(node);
    return NULL;
  }

  node->create_table.columns = NULL;

  if (!expect_token(tokens, 3, TOKEN_PUNCTUATION, PUNCT_OPEN_PAREN)) {
    PARSER_LOG_ERROR(tokens[3].position, PARSER_MISSING_DELIMITER,
                     tokens[3].value, PUNCT_OPEN_PAREN);
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
    printf("Failed to allocate columns\n");
    return NULL;
  }

  *num_columns = 0;
  while (*index < token_count) {
    if (tokens[*index].type != TOKEN_IDENTIFIER) {
      PARSER_LOG_ERROR(tokens[*index].position, PARSER_INVALID_IDENTIFIER,
                       tokens[*index].value, "column_name");
      columns_free(columns, *num_columns);
      return NULL;
    }

    columns[*num_columns].name = strdup(tokens[*index].value);
    if (!columns[*num_columns].name) {
      printf("Failed to duplicate column name\n");
      columns_free(columns, *num_columns);
      return NULL;
    }
    (*index)++;

    if (*index >= token_count || tokens[*index].type != TOKEN_KEYWORD) {
      PARSER_LOG_ERROR(tokens[*index].position, PARSER_INVALID_TYPE,
                       tokens[*index].value, "column_type");
      free(columns[*num_columns].name);
      columns_free(columns, *num_columns);
      return NULL;
    }

    columns[*num_columns].type = map_column_type(tokens[*index].value);
    (*index)++;
    (*num_columns)++;

    if (*index >= token_count || tokens[*index].type != TOKEN_PUNCTUATION) {
      PARSER_LOG_ERROR(tokens[*index].position, PARSER_MISSING_DELIMITER,
                       tokens[*index].value, "',' or ')'");
      columns_free(columns, *num_columns);
      return NULL;
    }

    if (strcmp(tokens[*index].value, PUNCT_COMMA) == 0) {
      (*index)++;
    } else if (strcmp(tokens[*index].value, PUNCT_CLOSE_PAREN) == 0) {
      (*index)++;
      break;
    } else {
      PARSER_LOG_ERROR(tokens[*index].position, PARSER_MISSING_DELIMITER,
                       tokens[*index].value, "',' or ')'");
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
