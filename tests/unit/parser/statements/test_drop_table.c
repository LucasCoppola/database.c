#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

#include "../../utils/test_utils.h"

void test_drop_table_query(bool should_pass, const char *expected_table_name) {
  if (should_pass) {
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, node->table_name);
  } else {
    TEST_ASSERT_NULL(node);
  }
}

void test_valid_drop_table(void) {
  current_query = "DROP TABLE users;";
  setUp();
  test_drop_table_query(true, "users");
}

void test_missing_table_name(void) {
  current_query = "DROP TABLE;";
  setUp();
  test_drop_table_query(false, NULL);
}

void test_invalid_drop_table(void) {
  current_query = "DROP users;";
  setUp();
  test_drop_table_query(false, NULL);
}

void run_drop_table_tests(void) {
  printf("\n");
  RUN_TEST(test_valid_drop_table);
  printf("\n");
  RUN_TEST(test_missing_table_name);
  printf("\n");
  RUN_TEST(test_invalid_drop_table);
}
