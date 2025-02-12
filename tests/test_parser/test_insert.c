#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/ast.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "../test_utils.h"

void test_insert(const char *query, bool should_pass,
                 const char *expected_table_name, const char **expected_values,
                 int expected_num_values) {
  printf("Testing query: %s\n", query);

  TokenizerState *state = setup_tokenizer(query);
  if (!state) {
    return;
  }

  ASTNode *node = parser_row_insert(state->tokens, state->token_count);

  if (should_pass) {
    if (!node) {
      printf("   FAIL: Expected parsing to succeed, but it failed.\n");
    } else {
      if (strcmp(node->table_name, expected_table_name) != 0) {
        printf("   FAIL: Expected table name '%s', got '%s'.\n",
               expected_table_name, node->table_name);
      } else if (node->insert_rows.num_values != expected_num_values) {
        printf("   FAIL: Expected %d values, got %d.\n", expected_num_values,
               node->insert_rows.num_values);
      } else {
        bool values_match = true;
        for (int i = 0; i < expected_num_values; i++) {
          if (strcmp(node->insert_rows.values[i], expected_values[i]) != 0) {
            values_match = false;
            break;
          }
        }
        if (!values_match) {
          printf("   FAIL: Values do not match expected values.\n");
        } else {
          printf("   PASS: Parsing succeeded and output matches expected "
                 "values.\n");
        }
      }
      ast_free(node);
    }
  } else {
    if (node) {
      printf("   FAIL: Expected parsing to fail, but it succeeded.\n");
      ast_free(node);
    } else {
      printf("   PASS: Parsing failed as expected.\n");
    }
  }

  teardown_tokenizer(state);
  printf("\n");
}

void run_insert_tests() {
  const char *query1 = "INSERT INTO users VALUES (1, 'Alice');";
  const char *expected_values1[] = {"1", "Alice"};
  test_insert(query1, true, "users", expected_values1, 2);

  const char *query2 = "INSERT INTO products VALUES (101, 'Laptop', 999.99);";
  const char *expected_values2[] = {"101", "Laptop", "999.99"};
  test_insert(query2, true, "products", expected_values2, 3);

  const char *query3 = "INSERT INTO users VALUES ();";
  test_insert(query3, false, NULL, NULL, 0);

  const char *query4 = "INSERT INTO users;";
  test_insert(query4, false, NULL, NULL, 0);

  const char *query5 = "INSERT INTO users VALUES (1, Alice;";
  test_insert(query5, false, NULL, NULL, 0);

  const char *query6 = "INSERT INTO VALUES (1, Alice);";
  test_insert(query6, false, NULL, NULL, 0);
}
