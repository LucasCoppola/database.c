#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"
#include "parser/tokenizer.h"
#include "utils/logger.h"

TokenizerState *setup_tokenizer(const char *query) {
  TokenizerState *state = NULL;
  TokenizerResult result = tokenizer_init(query, &state);
  if (result != TOKENIZER_SUCCESS) {
    LOG_ERROR("tokenizer", "init", result);
    return NULL;
  }

  TokenizerResult tokenize_query_result = tokenize_query(state);
  if (tokenize_query_result != TOKENIZER_SUCCESS) {
    if (tokenize_query_result == TOKENIZER_UNTERMINATED_STATEMENT) {
      fprintf(stderr, "Syntax Error: Unterminated statement at position %d.\n",
              state->position);
    } else {
      LOG_ERROR("tokenizer", "tokenize_query", tokenize_query_result);
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

void print_test_result(const char *test_name, bool passed) {
  printf("%s: %s\n", test_name, passed ? "PASS" : "FAIL");
}

DataType parse_data_type(const char *type) {
  if (strcmp(type, "INT") == 0)
    return COLUMN_TYPE_INT;
  if (strcmp(type, "TEXT") == 0)
    return COLUMN_TYPE_TEXT;
  return -1;
}
