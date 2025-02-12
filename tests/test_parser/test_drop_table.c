#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "../test_utils.h"

void test_drop_table(const char *query, bool should_pass,
                     const char *expected_table_name) {
  printf("Testing query: %s\n", query);

  TokenizerState *state = setup_tokenizer(query);
  if (!state)
    return;

  ASTNode *node = parser_table_drop(state->tokens);

  if (should_pass) {
    if (!node) {
      printf("   FAIL: Expected parsing to succeed, but it failed.\n");
    } else {
      if (strcmp(node->table_name, expected_table_name) != 0) {
        printf("   FAIL: Expected table name '%s', got '%s'.\n",
               expected_table_name, node->table_name);
      } else {
        printf("   PASS: Parsing succeeded and output matches expected "
               "values.\n");
      }
    }
  } else {
    if (node) {
      printf("   FAIL: Expected parsing to fail, but it succeeded.\n");
    } else {
      printf("   PASS: Parsing failed as expected.\n");
    }
  }

  if (node) {
    free(node);
  }
  teardown_tokenizer(state);
  printf("\n");
}

void run_drop_table_tests() {
  const char *query1 = "DROP TABLE users;";
  test_drop_table(query1, true, "users");

  const char *query2 = "DROP TABLE;";
  test_drop_table(query2, false, NULL);

  const char *query3 = "DROP users;";
  test_drop_table(query3, false, NULL);
}
