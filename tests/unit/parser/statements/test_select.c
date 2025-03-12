#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/statements.h"

#include "../../../common/test_context.h"

static TestContext ctx;

void setUp(void) {
  ctx.query = NULL;
  ctx.state = NULL;
  ctx.node = NULL;
}

void tearDown(void) { test_context_teardown(&ctx); }

void test_tokenizer_query(bool should_pass, const char *expected_table_name,
                          const char **expected_columns,
                          int expected_num_columns, bool expected_select_all) {
  if (ctx.state == NULL) {
    TEST_ASSERT_FALSE(should_pass);
    return;
  }

  ctx.node = parser_row_select(ctx.state->tokens, ctx.state->token_count);

  if (should_pass) {
    TEST_ASSERT_NOT_NULL(ctx.node);
    TEST_ASSERT_EQUAL_STRING(expected_table_name, ctx.node->table_name);
    TEST_ASSERT_EQUAL(expected_num_columns, ctx.node->select_rows.num_columns);
    TEST_ASSERT_EQUAL(expected_select_all, ctx.node->select_rows.select_all);

    if (!expected_select_all) {
      for (int i = 0; i < expected_num_columns; i++) {
        TEST_ASSERT_EQUAL_STRING(expected_columns[i],
                                 ctx.node->select_rows.select_columns[i]);
      }
    }
  } else {
    TEST_ASSERT_NULL(ctx.node);
  }
}

void test_valid_query(void) {
  const char *query = "SELECT id, name FROM users;";
  printf("Testing query: %s\n", query);

  test_context_init(&ctx, query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  const char *expected_columns[] = {"id", "name"};
  test_tokenizer_query(true, "users", expected_columns, 2, false);
}

void test_unterminated_query(void) {
  const char *query = "SELECT id, name FROM users";
  printf("Testing query: %s\n", query);

  test_context_init(&ctx, query);
  test_tokenizer_query(false, NULL, NULL, 0, false);
}

void test_invalid_query(void) {
  const char *query = "SELECT id, name FROM users";
  printf("Testing query: %s\n", query);

  test_context_init(&ctx, query);
  test_tokenizer_query(false, NULL, NULL, 0, false);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_valid_query);
  printf("\n");
  RUN_TEST(test_unterminated_query);
  printf("\n");
  RUN_TEST(test_invalid_query);

  return UNITY_END();
}
