#include <stdbool.h>
#include <stdio.h>

#include "parser/tokenizer.h"

bool test_query(const char *query, int query_num) {
  TokenizerState *state = NULL;
  TokenizerResult result = tokenizer_init(query, &state);

  if (result != TOKENIZER_SUCCESS) {
    printf("Query %d: Failed to initialize tokenizer\n", query_num);
    return false;
  }

  result = tokenize_query(state);
  tokenizer_free(state);

  if (result != TOKENIZER_SUCCESS) {
    printf("Query %d: Failed to tokenize\n", query_num);
    return false;
  }

  printf("Query %d: Passed\n", query_num);
  return true;
}

int main() {
  const char *queries[] = {
      "CREATE TABLE users (id INT, username TEXT, email TEXT);",
      "INSERT INTO users (id, username, email) VALUES (1, 'alice', "
      "'alice@example.com');",
      "INSERT INTO users (id, username, email) VALUES (2, 'bob', "
      "'bob@example.com');",
      "SELECT * FROM users;",
      "SELECT username, email FROM users;",
      "UPDATE users SET email = 'alice@newdomain.com' WHERE id = 1;",
      "DELETE FROM users WHERE id = 2;"};

  const int num_queries = sizeof(queries) / sizeof(queries[0]);
  int failed_tests = 0;

  for (int i = 0; i < num_queries; i++) {
    if (!test_query(queries[i], i + 1)) {
      failed_tests++;
    }
  }

  printf("\nTest Summary: %d passed, %d failed\n", num_queries - failed_tests,
         failed_tests);
  return failed_tests > 0 ? 1 : 0;
}
