#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/query_parser.h"

TokenizerState *tokenizer_init(const char *query) {
  TokenizerState *state = malloc(sizeof(TokenizerState));
  if (!state) {
    return NULL;
  }

  state->query = strdup(query);
  state->tokens = malloc(0);
  state->token_count = 0;
  state->position = 0;
  state->length = strlen(query);
  return state;
}

TokenizerResult tokenize_query(TokenizerState *state) {
  Token **tokens = &state->tokens;

  while (state->position <= state->length) {
    char c = state->query[state->position];

    if (isspace(c)) {
      state->position++;
      continue;
    }

    if (isalpha(c) || c == '_') { // Start of keyword or identifier
      int start_pos = state->position;
      char *word = read_word(state->query, &state->position);

      TokenType type = is_keyword(word) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
      add_token(state, word, type, start_pos);
      continue;
    }

    // Check for numeric literals
    if (isdigit(c) ||
        (c == '-' && isdigit(state->query[state->position + 1]))) {
      int start_pos = state->position;
      char *number = read_numeric_literal(state->query, &state->position);
      if (number) {
        add_token(state, number, TOKEN_LITERAL, start_pos);
      }
      continue;
    }

    // Check for string literals (single or double-quoted)
    // if (c == '\'' || c == '"') {
    //   int start_pos = state->position;
    //   char *string = read_string_literal(state->query, &state->position,
    //                                      c); // Handle quoted strings
    //   add_token(state, string, TOKEN_LITERAL, start_pos);
    //   continue;
    // }

    state->position++;
  }

  return TOKENIZER_SUCCESS;
}

void tokenizer_free(TokenizerState *state) {
  free(state->query);
  free(state->tokens);
  free(state);
}
