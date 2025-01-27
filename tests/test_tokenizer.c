#include <stdbool.h>
#include <stdio.h>

#include "../include/query_parser.h"

void print_tokens(TokenizerState *state) {
  printf("Length: %d\n", state->length);
  printf("Tokens:\n");
  for (int i = 0; i < state->token_count; i++) {
    Token *token = &state->tokens[i];
    const char *type_str = token->type == TOKEN_KEYWORD      ? "KEYWORD"
                           : token->type == TOKEN_IDENTIFIER ? "IDENTIFIER"
                           : token->type == TOKEN_LITERAL    ? "LITERAL"
                                                             : "UNKNOWN";

    printf("  [%d] Type: %s, Value: '%s', Position: %d\n", i, type_str,
           token->value, token->position);
  }
}

int main() {
  // const char *query = "CREATE TABLE users 123 -456 3.14 'is this working'";
  // const char *query = "CREATE TABLE users 'unterminated";
  // const char *query = "'it\\'s escaped'";
  // const char *query = "''";
  const char *query = "CREATE 123 'valid' invalid_token -34.56 'incomplete";

  TokenizerState *state = tokenizer_init(query);
  if (!state) {
    fprintf(stderr, "Failed to initialize tokenizer.\n");
    return 1;
  }

  tokenize_query(state);
  print_tokens(state);

  tokenizer_free(state);
  return 0;
}
