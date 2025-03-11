#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/test_utils.h"
#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

void test_tokenizer_query(bool should_pass, const char *expected_table_name,
                          const char **expected_columns,
                          int expected_num_columns, bool expected_select_all) {
  if (should_pass) {
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, node->table_name);
    TEST_ASSERT_EQUAL(expected_num_columns, node->select_rows.num_columns);
    TEST_ASSERT_EQUAL(expected_select_all, node->select_rows.select_all);

    if (!expected_select_all) {
      for (int i = 0; i < expected_num_columns; i++) {
        TEST_ASSERT_EQUAL_STRING(expected_columns[i],
                                 node->select_rows.select_columns[i]);
      }
    }
  } else {
    TEST_ASSERT_NULL(node);
  }
}

void test_valid_query(void) {
  current_query = "SELECT id, name FROM users;";
  setUp();
  const char *expected_columns[] = {"id", "name"};
  test_tokenizer_query(true, "users", expected_columns, 2, false);
}

void test_unterminated_query(void) {
  current_query = "SELECT id, name FROM users";
  setUp();
  test_tokenizer_query(false, NULL, NULL, 0, false);
}

void test_invalid_query(void) {
  current_query = "SELECT FROM users;";
  setUp();
  test_tokenizer_query(false, NULL, NULL, 0, false);
}

void run_select_tests(void) {
  printf("\n");
  RUN_TEST(test_valid_query);
  printf("\n");
  RUN_TEST(test_unterminated_query);
  printf("\n");
  RUN_TEST(test_invalid_query);
}
