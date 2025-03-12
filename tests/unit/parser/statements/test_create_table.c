#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

#include "../../../common/test_parser_ctx.h"

static TestParserCtx ctx;

void setUp(void) {
  ctx.query = NULL;
  ctx.state = NULL;
  ctx.node = NULL;
}

void tearDown(void) { test_parser_context_teardown(&ctx); }

void test_create_table_query(bool should_pass, const char *expected_table_name,
                             const char **expected_columns,
                             int expected_num_columns) {
  if (ctx.state == NULL) {
    TEST_ASSERT_FALSE(should_pass);
    return;
  }

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);

  if (should_pass) {
    TEST_ASSERT_NOT_NULL(ctx.node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, ctx.node->table_name);
    TEST_ASSERT_EQUAL(expected_num_columns, ctx.node->create_table.num_columns);

    for (int i = 0; i < expected_num_columns; i++) {
      const char *expected_name = expected_columns[2 * i];
      DataType expected_type = parse_data_type(expected_columns[2 * i + 1]);

      TEST_ASSERT_EQUAL_STRING(expected_name,
                               ctx.node->create_table.columns[i].name);
      TEST_ASSERT_EQUAL(expected_type, ctx.node->create_table.columns[i].type);
    }
  } else {
    TEST_ASSERT_NULL(ctx.node);
  }
}

void test_valid_create_table(void) {
  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  const char *expected_columns[] = {"id", "INT", "name", "TEXT"};
  test_create_table_query(true, "users", expected_columns, 2);
}

void test_valid_create_table_mixed_types(void) {
  const char *query = "CREATE TABLE mixed_types (id INT, name TEXT, price "
                      "REAL, is_active BOOLEAN);";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  const char *expected_columns[] = {"id",    "INT",  "name",      "TEXT",
                                    "price", "REAL", "is_active", "BOOLEAN"};
  test_create_table_query(true, "mixed_types", expected_columns, 4);
}

void test_invalid_create_table_missing_type(void) {
  const char *query = "CREATE TABLE invalid (id INT, name);";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_create_table_query(false, NULL, NULL, 0);
}

void test_invalid_create_table_missing_paren(void) {
  const char *query = "CREATE TABLE missing_paren (id INT, name TEXT";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_create_table_query(false, NULL, NULL, 0);
}

void test_invalid_create_table_empty(void) {
  const char *query = "CREATE TABLE empty ();";
  printf("Testing query: %s\n", query);

  test_parser_context_init(&ctx, query);
  test_create_table_query(false, NULL, NULL, 0);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_valid_create_table);
  printf("\n");
  RUN_TEST(test_invalid_create_table_missing_type);
  printf("\n");
  RUN_TEST(test_valid_create_table_mixed_types);
  printf("\n");
  RUN_TEST(test_invalid_create_table_missing_paren);
  printf("\n");
  RUN_TEST(test_invalid_create_table_empty);

  return UNITY_END();
}
