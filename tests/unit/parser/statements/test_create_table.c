#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

#include "../../utils/test_utils.h"

void test_create_table_query(bool should_pass, const char *expected_table_name,
                             const char **expected_columns,
                             int expected_num_columns) {
  if (should_pass) {
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, node->table_name);
    TEST_ASSERT_EQUAL(expected_num_columns, node->create_table.num_columns);

    for (int i = 0; i < expected_num_columns; i++) {
      const char *expected_name = expected_columns[2 * i];
      DataType expected_type = parse_data_type(expected_columns[2 * i + 1]);

      TEST_ASSERT_EQUAL_STRING(expected_name,
                               node->create_table.columns[i].name);
      TEST_ASSERT_EQUAL(expected_type, node->create_table.columns[i].type);
    }
  } else {
    TEST_ASSERT_NULL(node);
  }
}

void test_valid_create_table(void) {
  current_query = "CREATE TABLE users (id INT, name TEXT);";
  setUp();
  const char *expected_columns[] = {"id", "INT", "name", "TEXT"};
  test_create_table_query(true, "users", expected_columns, 2);
}

void test_valid_create_table_mixed_types(void) {
  current_query = "CREATE TABLE mixed_types (id INT, name TEXT, price REAL, "
                  "is_active BOOLEAN);";
  setUp();
  const char *expected_columns[] = {"id",    "INT",  "name",      "TEXT",
                                    "price", "REAL", "is_active", "BOOLEAN"};
  test_create_table_query(true, "mixed_types", expected_columns, 4);
}

void test_invalid_create_table_missing_type(void) {
  current_query = "CREATE TABLE invalid (id INT, name);";
  setUp();
  test_create_table_query(false, NULL, NULL, 0);
}

void test_invalid_create_table_missing_paren(void) {
  current_query = "CREATE TABLE missing_paren (id INT, name TEXT";
  setUp();
  test_create_table_query(false, NULL, NULL, 0);
}

void test_invalid_create_table_empty(void) {
  current_query = "CREATE TABLE empty ();";
  setUp();
  test_create_table_query(false, NULL, NULL, 0);
}

void run_create_table_tests(void) {
  printf("\n");
  RUN_TEST(test_valid_create_table);
  printf("\n");
  RUN_TEST(test_invalid_create_table_missing_type);
  printf("\n");
  RUN_TEST(test_valid_create_table_mixed_types);
  printf("\n");
  RUN_TEST(test_invalid_create_table_missing_paren);
  printf("\n");
  RUN_TEST(test_invalid_create_table_empty);
}
