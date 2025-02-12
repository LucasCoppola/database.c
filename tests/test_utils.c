#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"
#include "parser/tokenizer.h"

TokenizerState *setup_tokenizer(const char *query) {
  TokenizerState *state = NULL;
  TokenizerResult result = tokenizer_init(query, &state);
  if (result != TOKENIZER_SUCCESS) {
    fprintf(stderr, "Failed to initialize tokenizer.\n");
    return NULL;
  }
  tokenize_query(state);
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
    return TYPE_INT;
  if (strcmp(type, "TEXT") == 0)
    return TYPE_TEXT;
  return -1;
}
