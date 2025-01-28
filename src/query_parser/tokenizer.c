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

  while (state->position < state->length) {
    char c = state->query[state->position];

    if (isspace(c)) {
      state->position++;
      continue;
    }

    // keyword or identifier
    if (isalpha(c) || c == '_') {
      int start_pos = state->position;
      char *word = read_word(state->query, &state->position);

      TokenType type = is_keyword(word) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
      add_token(state, word, type, start_pos);
      continue;
    }

    // numeric literals
    if (isdigit(c) ||
        (c == '-' && isdigit(state->query[state->position + 1]))) {
      int start_pos = state->position;
      char *number = read_numeric_literal(state->query, &state->position);
      if (number) {
        add_token(state, number, TOKEN_LITERAL, start_pos);
      } else {
        printf("Error thrown at read numeric literal\n");
        return TOKENIZER_ERROR;
      }
      continue;
    }

    // string literals (single or double-quoted)
    if (c == '\'' || c == '"') {
      int start_pos = state->position;
      char *string = read_string_literal(state->query, &state->position, c);
      if (string) {
        add_token(state, string, TOKEN_LITERAL, start_pos);
      } else {
        printf("Error thrown at read string literal\n");
        return TOKENIZER_ERROR;
      }
      continue;
    }

    if (is_operator(c)) {
      char operator_str[2] = {c, '\0'};
      add_token(state, operator_str, TOKEN_OPERATOR, state->position);
      state->position++;
      continue;
    }

    if (is_punctuation(c)) {
      char punctuation_str[2] = {c, '\0'};
      add_token(state, punctuation_str, TOKEN_PUNCTUATION, state->position);
      state->position++;
      continue;
    }

    if (c == '*') {
      char wildcard_str[2] = {c, '\0'};
      add_token(state, wildcard_str, TOKEN_WILDCARD, state->position);
      state->position++;
      continue;
    }

    if (c == ';') {
      char eof_str[2] = {c, '\0'};
      add_token(state, eof_str, TOKEN_EOF, state->position);
      return TOKENIZER_SUCCESS;
    }

    state->position++;
  }

  return TOKENIZER_ERROR;
}

void tokenizer_free(TokenizerState *state) {
  for (size_t i = 0; i < state->token_count; i++) {
    free(state->tokens[i].value);
  }
  free(state->tokens);
  free(state->query);
  free(state);
}
