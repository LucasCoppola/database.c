#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"
#include "parser/tokenizer.h"
#include "test_utils.h"
#include "utils/logger.h"

#include "core/row.h"

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
