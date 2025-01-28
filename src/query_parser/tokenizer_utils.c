#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/query_parser.h"

const char *keywords[] = {"CREATE", "DROP", "TABLE", "SELECT", "INSERT",
                          "DELETE", "FROM", "WHERE", "INTO",   "VALUES"};

bool is_keyword(char *value) {
  for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
    if (strcasecmp(value, keywords[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_operator(char value) {
  return (value == '=' || value == '>' || value == '<');
}

bool is_punctuation(char value) {
  return (value == ',' || value == '(' || value == ')');
}

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
  token->value = strdup(value);
  if (!token->value) {
    perror("Failed to allocate memory for token value");
    exit(1);
  }
  token->position = position;

  state->token_count++;
}

char *read_word(char *query, int *position) {
  int start_pos = *position;
  while (isalnum(query[*position]) || query[*position] == '_') {
    (*position)++;
  }

  int length = *position - start_pos;

  char *value = malloc(length + 1);
  if (!value) {
    perror("Failed to allocate memory for word");
    return NULL;
  }
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

  char *value = malloc(length + 1);
  if (!value) {
    perror("Failed to allocate memory for numeric literal");
    return NULL;
  }
  strncpy(value, &query[start_pos], length);

  value[length] = '\0';
  return value;
}

char *read_string_literal(char *query, int *position, char quote) {
  int start_pos = *position;
  (*position)++; // opening quote

  int capacity = 16;
  int length = 0;
  char *value = malloc(capacity * sizeof(char));
  if (value == NULL) {
    *position = start_pos;
    return NULL;
  }

  char c = query[*position];
  bool escape = false;

  while (c != quote || escape) {
    if (length >= capacity - 1) {
      capacity *= 2;
      char *new_value = (char *)realloc(value, capacity * sizeof(char));
      if (new_value == NULL) {
        free(value);
        *position = start_pos;
        return NULL;
      }
      value = new_value;
    }

    if (escape) {
      switch (c) {
      case 'n':
        value[length++] = '\n';
        break;
      case 't':
        value[length++] = '\t';
        break;
      case 'r':
        value[length++] = '\r';
        break;
      case '\\':
        value[length++] = '\\';
        break;
      case '\'':
        value[length++] = '\'';
        break;
      case '"':
        value[length++] = '"';
        break;
      default:
        value[length++] = c;
        break;
      }
      escape = false;
    } else if (c == '\\') {
      escape = true;
    } else {
      value[length++] = c;
    }

    (*position)++;
    c = query[*position];
  }

  // end of string (unmatched quotes)
  if (c != quote) {
    free(value);
    *position = start_pos;
    return NULL;
  }

  // closing quote
  (*position)++;

  value[length] = '\0';
  return value;
}
