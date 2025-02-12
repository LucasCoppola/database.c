#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/tokenizer.h"

TokenizerResult tokenizer_init(const char *query, TokenizerState **state) {
  *state = (TokenizerState *)malloc(sizeof(TokenizerState));
  if (!*state) {
    return TOKENIZER_ALLOC_ERROR;
  }

  (*state)->query = strdup(query);
  if (!(*state)->query) {
    free(*state);
    *state = NULL;
    return TOKENIZER_ALLOC_ERROR;
  }

  (*state)->tokens = calloc(1, sizeof(Token));
  if (!(*state)->tokens) {
    free((*state)->query);
    free(*state);
    *state = NULL;
    return TOKENIZER_ALLOC_ERROR;
  }

  (*state)->token_count = 0;
  (*state)->position = 0;
  (*state)->length = strlen(query);
  return TOKENIZER_SUCCESS;
}

TokenizerResult tokenize_query(TokenizerState *state) {
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
      if (!word) {
        fprintf(stderr, "Error: Failed to read word at position %d\n",
                start_pos);
        return TOKENIZER_UNKNOWN_ERROR;
      }

      TokenType type = is_keyword(word) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
      add_token(state, word, type, start_pos);
      free(word);
      continue;
    }

    // numeric literals
    if (isdigit(c) ||
        (c == '-' && isdigit(state->query[state->position + 1]))) {
      int start_pos = state->position;
      char *number = read_numeric_literal(state->query, &state->position);
      if (!number) {
        fprintf(stderr, "Error: Invalid numeric literal at position %d\n",
                start_pos);
        return TOKENIZER_UNKNOWN_ERROR;
      }

      add_token(state, number, TOKEN_LITERAL, start_pos);
      free(number);
      continue;
    }

    // string literals (single or double-quoted)
    if (c == '\'' || c == '"') {
      int start_pos = state->position;
      char *string = read_string_literal(state->query, &state->position, c);
      if (!string) {
        fprintf(stderr, "Error: Unterminated string literal at position %d\n",
                start_pos);
        return TOKENIZER_UNKNOWN_ERROR;
      }

      add_token(state, string, TOKEN_LITERAL, start_pos);
      free(string);
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
    fprintf(stderr, "Warning: Unrecognized character '%c' at position %d\n", c,
            state->position);
    state->position++;
  }

  return TOKENIZER_UNKNOWN_ERROR;
}

void tokenizer_free(TokenizerState *state) {
  for (int i = 0; i < state->token_count; i++) {
    free(state->tokens[i].value);
  }
  free(state->tokens);
  free(state->query);
  free(state);
}
