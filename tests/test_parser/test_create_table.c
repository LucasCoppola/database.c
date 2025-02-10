#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ast.h"
#include "../include/statements.h"
#include "../include/tokenizer.h"
#include "../test_utils.h"

void test_create_table(const char *query, bool should_pass,
                       const char *expected_table_name,
                       const char **expected_columns,
                       int expected_num_columns) {
  printf("Testing query: %s\n", query);

  TokenizerState *state = setup_tokenizer(query);
  if (!state)
    return;

  ASTNode *node = parser_table_create(state->tokens, state->token_count);

  if (should_pass) {
    if (!node) {
      printf("   FAIL: Expected parsing to succeed, but it failed.\n");
    } else {
      if (strcmp(node->table_name, expected_table_name) != 0) {
        printf("   FAIL: Expected table name '%s', got '%s'.\n",
               expected_table_name, node->table_name);
      } else if (node->create_table.num_columns != expected_num_columns) {
        printf("   FAIL: Expected %d columns, got %d.\n",
               expected_num_columns, node->create_table.num_columns);
      } else {
        bool columns_match = true;
        for (int i = 0; i < expected_num_columns; i++) {
          const char *expected_name = expected_columns[2 * i];
          DataType expected_type = parse_data_type(expected_columns[2 * i + 1]);
          if (strcmp(node->create_table.columns[i].name, expected_name) != 0 ||
              node->create_table.columns[i].type != expected_type) {
            columns_match = false;
            break;
          }
        }
        if (!columns_match) {
          printf("   FAIL: Columns do not match expected values.\n");
        } else {
          printf("   PASS: Parsing succeeded and output matches expected "
                 "values.\n");
        }
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
    ast_free(node);
  }
  teardown_tokenizer(state);
  printf("\n");
}

void run_create_table_tests() {
  const char *query1 = "CREATE TABLE users (id INT, name TEXT);";
  const char *expected_columns1[] = {"id", "INT", "name", "TEXT"};
  test_create_table(query1, true, "users", expected_columns1, 2);

  const char *query2 =
      "CREATE TABLE products (product_id INT, product_name TEXT, price INT);";
  const char *expected_columns2[] = {"product_id", "INT",   "product_name",
                                     "TEXT",       "price", "INT"};
  test_create_table(query2, true, "products", expected_columns2, 3);

  const char *query3 = "CREATE TABLE invalid (id INT, name);";
  test_create_table(query3, false, NULL, NULL, 0);

  const char *query4 = "CREATE TABLE missing_paren (id INT, name TEXT";
  test_create_table(query4, false, NULL, NULL, 0);

  const char *query5 = "CREATE TABLE empty ();";
  test_create_table(query5, false, NULL, NULL, 0);
}
