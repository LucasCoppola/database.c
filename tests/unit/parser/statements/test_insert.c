#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

#include "../../utils/test_utils.h"
#include "core/row.h"

void test_insert_query(bool should_pass, const char *expected_table_name,
                       const char **expected_values, int expected_num_values) {
  if (should_pass) {
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, node->table_name);
    TEST_ASSERT_EQUAL(expected_num_values, node->insert_rows.num_values);

    for (int i = 0; i < expected_num_values; i++) {
      char actual_value[32];
      switch (node->insert_rows.values[i].type) {
      case COLUMN_TYPE_TEXT:
        strncpy(actual_value, node->insert_rows.values[i].string_value, 32);
        break;
      case COLUMN_TYPE_INT:
        snprintf(actual_value, 32, "%d", node->insert_rows.values[i].int_value);
        break;
      case COLUMN_TYPE_REAL:
        snprintf(actual_value, 32, "%.2f",
                 node->insert_rows.values[i].real_value);
        break;
      case COLUMN_TYPE_BOOL:
        snprintf(actual_value, 32, "%s",
                 node->insert_rows.values[i].bool_value ? "true" : "false");
        break;
      default:
        snprintf(actual_value, 32, "unknown_type");
      }
      TEST_ASSERT_EQUAL_STRING(expected_values[i], actual_value);
    }
  } else {
    TEST_ASSERT_NULL(node);
  }
}

void test_valid_insert_multiple_values(void) {
  current_query = "INSERT INTO products VALUES (101, 'Laptop', 999.99, false);";
  setUp();
  const char *expected_values[] = {"101", "Laptop", "999.99", "false"};
  test_insert_query(true, "products", expected_values, 4);
}

void test_insert_missing_values(void) {
  current_query = "INSERT INTO users VALUES ();";
  setUp();
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_missing_table(void) {
  current_query = "INSERT INTO users;";
  setUp();
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_unterminated_values(void) {
  current_query = "INSERT INTO users VALUES (1, Alice;";
  setUp();
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_missing_table_name(void) {
  current_query = "INSERT INTO VALUES (1, Alice);";
  setUp();
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_invalid_boolean_format(void) {
  current_query = "INSERT INTO flags VALUES ('feature', truth);";
  setUp();
  test_insert_query(false, NULL, NULL, 0);
}

void run_insert_tests(void) {
  printf("\n");
  RUN_TEST(test_valid_insert_multiple_values);
  printf("\n");
  RUN_TEST(test_insert_missing_values);
  printf("\n");
  RUN_TEST(test_insert_missing_table);
  printf("\n");
  RUN_TEST(test_insert_unterminated_values);
  printf("\n");
  RUN_TEST(test_insert_missing_table_name);
  printf("\n");
  RUN_TEST(test_insert_invalid_boolean_format);
}
