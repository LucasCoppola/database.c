#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/query_parser.h"

const char *keywords[] = {"CREATE", "DROP", "TABLE", "SELECT", "INSERT",
                          "DELETE", "FROM", "WHERE", "INTO"};

bool is_keyword(const char *value) {
  for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
    if (strcmp(value, keywords[i]) == 0) {
      return true;
    }
  }
  return false;
}

// Literals (e.g., strings, numbers).
// Operators (e.g., =, <, >).
// Punctuation (e.g., ,, (, )).

void add_token(TokenizerState *state, const char *value, TokenType type,
               int position) {
  state->tokens =
      realloc(state->tokens, (state->token_count + 1) * sizeof(Token));
  if (!state->tokens) {
    perror("Failed to allocate memory for tokens");
    exit(1);
  }

  Token *token = &state->tokens[state->token_count];
  token->type = type;
  strncpy(token->value, value, sizeof(token->value) - 1);
  token->value[sizeof(token->value) - 1] = '\0';
  token->position = position;

  state->token_count++;
}

char *read_word(char *query, int *position) {
  int start_pos = *position;
  while (isalnum(query[*position]) || query[*position] == '_') {
    (*position)++;
  }

  int length = *position - start_pos;
  static char value[256];

  strncpy(value, &query[start_pos], length);
  value[length] = '\0';
  return value;
}

char *read_numeric_literal(char *query, int *position) {
  int start_pos = *position;
  bool has_decimal = false;
  bool has_digits = false;

  // check for negative sign
  if (query[*position] == '-') {
    (*position)++;
  }

  while (isdigit(query[*position]) || query[*position] == '.') {
    if (query[*position] == '.') {
      // prevent 3..14
      if (has_decimal) {
        break;
      }
      has_decimal = true;
    } else {
      has_digits = true;
    }

    (*position)++;
  }

  // check if we've read a number not just a sign
  if (!has_digits) {
    *position = start_pos;
    return NULL;
  }

  int length = *position - start_pos;
  static char value[256];

  strncpy(value, &query[start_pos], length);
  value[length] = '\0';
  return value;
}

char *read_string_literal(char *query, int *position) {}
