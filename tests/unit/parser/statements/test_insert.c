#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

#include "../../../common/test_parser_ctx.h"
#include "core/row.h"

static TestParserCtx ctx;

void setUp(void) {
  ctx.query = NULL;
  ctx.state = NULL;
  ctx.node = NULL;
}

void tearDown(void) { test_parser_context_teardown(&ctx); }

void test_insert_query(bool should_pass, const char *expected_table_name,
                       const char **expected_values, int expected_num_values) {
  if (ctx.state == NULL) {
    TEST_ASSERT_FALSE(should_pass);
    return;
  }

  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);

  if (should_pass) {
    TEST_ASSERT_NOT_NULL(ctx.node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, ctx.node->table_name);
    TEST_ASSERT_EQUAL(expected_num_values, ctx.node->insert_rows.num_values);

    for (int i = 0; i < expected_num_values; i++) {
      char actual_value[32];
      switch (ctx.node->insert_rows.values[i].type) {
      case COLUMN_TYPE_TEXT:
        strncpy(actual_value, ctx.node->insert_rows.values[i].string_value, 32);
        break;
      case COLUMN_TYPE_INT:
        snprintf(actual_value, 32, "%d",
                 ctx.node->insert_rows.values[i].int_value);
        break;
      case COLUMN_TYPE_REAL:
        snprintf(actual_value, 32, "%.2f",
                 ctx.node->insert_rows.values[i].real_value);
        break;
      case COLUMN_TYPE_BOOL:
        snprintf(actual_value, 32, "%s",
                 ctx.node->insert_rows.values[i].bool_value ? "true" : "false");
        break;
      default:
        snprintf(actual_value, 32, "unknown_type");
      }
      TEST_ASSERT_EQUAL_STRING(expected_values[i], actual_value);
    }
  } else {
    TEST_ASSERT_NULL(ctx.node);
  }
}

void test_valid_insert_multiple_values(void) {
  const char *query =
      "INSERT INTO products VALUES (101, 'Laptop', 999.99, false);";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  const char *expected_values[] = {"101", "Laptop", "999.99", "false"};
  test_insert_query(true, "products", expected_values, 4);
}

void test_insert_missing_values(void) {
  const char *query = "INSERT INTO users VALUES ();";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_missing_table(void) {
  const char *query = "INSERT INTO users;";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_unterminated_values(void) {
  const char *query = "INSERT INTO users VALUES (1, Alice;";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_missing_table_name(void) {
  const char *query = "INSERT INTO VALUES (1, Alice);";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_insert_query(false, NULL, NULL, 0);
}

void test_insert_invalid_boolean_format(void) {
  const char *query = "INSERT INTO flags VALUES ('feature', truth);";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_insert_query(false, NULL, NULL, 0);
}

int main(void) {
  UNITY_BEGIN();

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

  return UNITY_END();
}
