#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../common/test_context.h"
#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

static TestContext ctx;

void setUp(void) {
  ctx.query = NULL;
  ctx.state = NULL;
  ctx.node = NULL;
}

void tearDown(void) { test_context_teardown(&ctx); }

void test_drop_table_query(bool should_pass, const char *expected_table_name) {
  if (ctx.state == NULL) {
    TEST_ASSERT_FALSE(should_pass);
    return;
  }

  ctx.node = parser_table_drop(ctx.state->tokens);

  if (should_pass) {
    TEST_ASSERT_NOT_NULL(ctx.node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, ctx.node->table_name);
  } else {
    TEST_ASSERT_NULL(ctx.node);
  }
}

void test_valid_drop_table(void) {
  const char *query = "DROP TABLE users;";
  printf("Testing query: %s\n", query);

  test_context_init(&ctx, query);
  test_drop_table_query(true, "users");
}

void test_missing_table_name(void) {
  const char *query = "DROP TABLE;";
  printf("Testing query: %s\n", query);

  test_context_init(&ctx, query);
  test_drop_table_query(false, NULL);
}

void test_invalid_drop_table(void) {
  const char *query = "DROP users;";
  printf("Testing query: %s\n", query);

  test_context_init(&ctx, query);
  test_drop_table_query(false, NULL);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_valid_drop_table);
  printf("\n");
  RUN_TEST(test_missing_table_name);
  printf("\n");
  RUN_TEST(test_invalid_drop_table);

  return UNITY_END();
}
