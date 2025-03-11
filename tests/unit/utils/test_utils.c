#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"
#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"
#include "test_utils.h"
#include "utils/logger.h"

char *current_query = NULL;
TokenizerState *state = NULL;
ASTNode *node = NULL;

void setUp(void) {
  if (current_query == NULL) {
    return;
  }

  printf("Testing query: %s\n", current_query);
  state = setup_tokenizer(current_query);
  if (state == NULL) {
    tearDown();
    return;
  }

  Token *tokens = state->tokens;
  if (state->token_count > 0 && tokens[0].type == TOKEN_KEYWORD) {
    if (strcmp(tokens[0].value, "INSERT") == 0) {
      node = parser_row_insert(state->tokens, state->token_count);
    } else if (strcmp(tokens[0].value, "CREATE") == 0) {
      node = parser_table_create(state->tokens, state->token_count);
    } else if (strcmp(tokens[0].value, "DROP") == 0) {
      node = parser_table_drop(state->tokens);
    } else if (strcmp(tokens[0].value, "SELECT") == 0) {
      node = parser_row_select(state->tokens, state->token_count);
    } else {
      node = NULL;
    }
  } else {
    node = NULL;
  }
}

void tearDown(void) {
  if (node) {
    ast_free(node);
    node = NULL;
  }

  if (state) {
    tokenizer_free(state);
    state = NULL;
  }

  current_query = NULL;
}

TokenizerState *setup_tokenizer(const char *query) {
  TokenizerState *state = NULL;
  TokenizerResult result = tokenizer_init(query, &state);
  if (result != TOKENIZER_SUCCESS) {
    DEBUG_LOG("tokenizer", "init", result);
    return NULL;
  }

  TokenizerResult tokenize_query_result = tokenize_query(state);
  if (tokenize_query_result != TOKENIZER_SUCCESS) {
    if (tokenize_query_result == TOKENIZER_UNTERMINATED_STATEMENT) {
      fprintf(stderr, "Syntax Error: Unterminated statement at position %d.\n",
              state->position);
    } else {
      DEBUG_LOG("tokenizer", "tokenize_query", tokenize_query_result);
    }
    tokenizer_free(state);
    return NULL;
  }

  return state;
}

void teardown_tokenizer(TokenizerState *state) {
  if (state) {
    tokenizer_free(state);
  }
}

DataType parse_data_type(const char *type_str) {
  if (strcmp(type_str, "INT") == 0) {
    return COLUMN_TYPE_INT;
  } else if (strcmp(type_str, "TEXT") == 0) {
    return COLUMN_TYPE_TEXT;
  } else if (strcmp(type_str, "REAL") == 0) {
    return COLUMN_TYPE_REAL;
  } else if (strcmp(type_str, "BOOLEAN") == 0) {
    return COLUMN_TYPE_BOOL;
  }
  return COLUMN_TYPE_UNKNOWN;
}
