#include <stdbool.h>
#include <stdio.h>

#include "../include/tokenizer.h"

void print_tokens(TokenizerState *state) {
  printf("Length: %d\n", state->length);
  printf("Tokens:\n");
  for (int i = 0; i < state->token_count; i++) {
    Token *token = &state->tokens[i];
    const char *type_str = token->type == TOKEN_KEYWORD       ? "KEYWORD"
                           : token->type == TOKEN_IDENTIFIER  ? "IDENTIFIER"
                           : token->type == TOKEN_LITERAL     ? "LITERAL"
                           : token->type == TOKEN_OPERATOR    ? "OPERATOR"
                           : token->type == TOKEN_PUNCTUATION ? "PUNCTUATION"
                           : token->type == TOKEN_WILDCARD    ? "WILDCARD"
                           : token->type == TOKEN_EOF         ? "EOF"
                                                              : "UNKNOWN";

    printf("  [%d] Type: %s, Value: '%s', Position: %d\n", i, type_str,
           token->value, token->position);
  }
}

int main() {
  const char *query1 =
      "CREATE TABLE users (id INT, username TEXT, email TEXT);";
  const char *query2 = "INSERT INTO users (id, username, email) VALUES (1, "
                       "'alice', 'alice@example.com');";
  const char *query3 = "INSERT INTO users (id, username, email) VALUES (2, "
                       "'bob', 'bob@example.com');";
  const char *query4 = "SELECT * FROM users;";
  const char *query5 = "SELECT username, email FROM users;";
  const char *query6 =
      "UPDATE users SET email = 'alice@newdomain.com' WHERE id = 1;";
  const char *query7 = "DELETE FROM users WHERE id = 2;";

  TokenizerState *state = NULL;
  TokenizerResult result = tokenizer_init(query2, &state);
  if (result != TOKENIZER_SUCCESS) {
    fprintf(stderr, "Failed to initialize tokenizer.\n");
    return 1;
  }

  tokenize_query(state);
  print_tokens(state);

  tokenizer_free(state);
  return 0;
}
