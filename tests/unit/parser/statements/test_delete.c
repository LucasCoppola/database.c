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

void test_valid_delete_without_where(void) {
  const char *query = "DELETE FROM users;";
  printf("Testing query: %s\n", query);
  test_parser_context_init(&ctx, query);

  ctx.node = parser_row_delete(ctx.state->tokens);
  TEST_ASSERT_NOT_NULL(ctx.node);
  TEST_ASSERT_EQUAL_STRING("users", ctx.node->table_name);
  TEST_ASSERT_TRUE(ctx.node->delete_rows.delete_all);
}

void test_valid_delete_with_where(void) {
  const char *query = "DELETE FROM users WHERE id = 1;";
  printf("Testing query: %s\n", query);
  test_parser_context_init(&ctx, query);

  ctx.node = parser_row_delete(ctx.state->tokens);
  TEST_ASSERT_NOT_NULL(ctx.node);
  TEST_ASSERT_EQUAL_STRING("users", ctx.node->table_name);
  TEST_ASSERT_FALSE(ctx.node->delete_rows.delete_all);

  TEST_ASSERT_NOT_NULL(ctx.node->where_condition.column_name);
  TEST_ASSERT_EQUAL_STRING("id", ctx.node->where_condition.column_name);
  TEST_ASSERT_EQUAL('=', ctx.node->where_condition.op);
  TEST_ASSERT_NOT_NULL(ctx.node->where_condition.value);
  TEST_ASSERT_EQUAL(COLUMN_TYPE_INT, ctx.node->where_condition.value->type);
  TEST_ASSERT_EQUAL(1, ctx.node->where_condition.value->int_value);
}

void test_invalid_delete_missing_identifier(void) {
  const char *query = "DELETE FROM ;";
  printf("Testing query: %s\n", query);
  test_parser_context_init(&ctx, query);

  ctx.node = parser_row_delete(ctx.state->tokens);
  TEST_ASSERT_NULL(ctx.node);
}

void test_invalid_delete_missing_from_keyword(void) {
  const char *query = "DELETE users;";
  printf("Testing query: %s\n", query);
  test_parser_context_init(&ctx, query);

  ctx.node = parser_row_delete(ctx.state->tokens);
  TEST_ASSERT_NULL(ctx.node);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_valid_delete_without_where);
  printf("\n");
  RUN_TEST(test_valid_delete_with_where);
  printf("\n");
  RUN_TEST(test_invalid_delete_missing_identifier);
  printf("\n");
  RUN_TEST(test_invalid_delete_missing_from_keyword);
  return UNITY_END();
}
