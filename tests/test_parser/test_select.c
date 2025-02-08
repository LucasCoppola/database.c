#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parser.h"
#include "../include/tokenizer.h"
#include "../test_utils.h"

void test_select(const char *query, bool should_pass,
                 const char *expected_table_name, const char **expected_columns,
                 int expected_num_columns, bool expected_select_all) {
  printf("Testing query: %s\n", query);
  TokenizerState *state = setup_tokenizer(query);
  if (!state)
    return;

  ASTNode *node = parser_row_select(state->tokens, state->token_count);

  if (should_pass) {
    if (!node) {
      printf("  FAIL: Expected parsing to succeed, but it failed.\n");
    } else {
      if (strcmp(node->table_name, expected_table_name) != 0) {
        printf("  FAIL: Expected table name '%s', got '%s'.\n",
               expected_table_name, node->table_name);
      } else if (node->select_rows.num_columns != expected_num_columns) {
        printf("  FAIL: Expected %d columns, got %d.\n", expected_num_columns,
               node->select_rows.num_columns);
      } else if (node->select_rows.select_all != expected_select_all) {
        printf("  FAIL: Expected select_all=%d, got %d.\n", expected_select_all,
               node->select_rows.select_all);
      } else {
        bool columns_match = true;
        for (int i = 0; i < expected_num_columns; i++) {
          if (strcmp(node->select_rows.select_columns[i],
                     expected_columns[i]) != 0) {
            columns_match = false;
            break;
          }
        }
        if (!columns_match) {
          printf("  FAIL: Columns do not match expected values.\n");
        } else {
          printf("  PASS: Parsing succeeded and output matches expected "
                 "values.\n");
        }
      }
      ast_free(node);
    }
  } else {
    if (node) {
      printf("  FAIL: Expected parsing to fail, but it succeeded.\n");
      ast_free(node);
    } else {
      printf("  PASS: Parsing failed as expected.\n");
    }
  }

  teardown_tokenizer(state);
  printf("\n");
}

void run_select_tests() {
  const char *query1 = "SELECT id, name FROM users;";
  const char *expected_columns1[] = {"id", "name"};
  test_select(query1, true, "users", expected_columns1, 2, false);

  const char *query2 = "SELECT * FROM products;";
  test_select(query2, true, "products", NULL, 0, true);

  const char *query3 = "SELECT id FROM;"; // Missing table name
  test_select(query3, false, NULL, NULL, 0, false);

  const char *query4 = "SELECT FROM users;"; // Missing columns
  test_select(query4, false, NULL, NULL, 0, false);
}
