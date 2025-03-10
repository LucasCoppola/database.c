#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/keywords.h"
#include "parser/tokenizer.h"

static const char *keywords[] = {
    KEYWORD_CREATE, KEYWORD_DROP,    KEYWORD_TABLE,
    KEYWORD_SELECT, KEYWORD_INSERT,  KEYWORD_DELETE,
    KEYWORD_FROM,   KEYWORD_INTO,    KEYWORD_VALUES,

    KEYWORD_INT,    KEYWORD_INTEGER, KEYWORD_REAL,
    KEYWORD_TEXT,   KEYWORD_BOOLEAN};

bool is_keyword(char *value) {
  int keywords_length = sizeof(keywords) / sizeof(keywords[0]);
  for (int i = 0; i < keywords_length; i++) {
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
  Token *new_tokens =
      realloc(state->tokens, (state->token_count + 1) * sizeof(Token));
  if (!new_tokens) {
    fprintf(stderr, "Failed to allocate memory for new token");
    tokenizer_free(state);
    exit(EXIT_FAILURE);
  }

  state->tokens = new_tokens;

  Token *token = &state->tokens[state->token_count];
  token->type = type;
  token->value = strdup(value);
  if (!token->value) {
    fprintf(stderr, "Failed to allocate memory for token value");
    tokenizer_free(state);
    exit(EXIT_FAILURE);
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
    return NULL;
  }
  strncpy(value, &query[start_pos], length);

  value[length] = '\0';

  if (is_keyword(value)) {
    for (int i = 0; value[i]; i++) {
      value[i] = toupper(value[i]);
    }
  }

  return value;
}

NumericLiteralResult read_numeric_literal(char *query, int *position) {
  int start_pos = *position;
  bool has_decimal = false;

  if (query[*position] == '-') {
    (*position)++;
  }

  while (isdigit(query[*position]) || query[*position] == '.') {
    if (query[*position] == '.') {
      if (has_decimal) {
        break; // Invalid format (e.g., "3.14.15")
      }
      has_decimal = true;
    }
    (*position)++;
  }

  int length = *position - start_pos;
  char *value = malloc(length + 1);
  if (!value) {
    fprintf(stderr, "Failed to allocate memory for numeric literal");
    return (NumericLiteralResult){NULL, false};
  }
  strncpy(value, &query[start_pos], length);
  value[length] = '\0';

  return (NumericLiteralResult){value, has_decimal};
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

  bool escape = false;

  while (query[*position] != '\0') {
    char c = query[*position];

    if (length >= capacity - 1) {
      capacity *= 2;
      char *new_value = realloc(value, capacity * sizeof(char));
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
      case '0':
        value[length++] = '\0';
        break;
      default:
        // Invalid escape sequence - treat as literal character
        value[length++] = '\\';
        value[length++] = c;
        break;
      }
      escape = false;
    } else if (c == '\\') {
      escape = true;
    } else if (c == quote && !escape) {
      // End of string found
      (*position)++; // Skip closing quote
      value[length] = '\0';
      return value;
    } else {
      value[length++] = c;
    }

    (*position)++;
  }

  // Unterminated string or escape sequence
  free(value);
  *position = start_pos;
  return NULL;
}
