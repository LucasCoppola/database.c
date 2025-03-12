#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"
#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/semantic_analyzer.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "core/row.h"
#include "executor/executor.h"
#include "storage/cursor.h"
#include "utils/logger.h"

#include "../common/test_integration_ctx.h"

static const char *TEST_DB = "test_insert_row.db";
static TestIntegrationCtx ctx;

Table *table = NULL;
Cursor *cursor = NULL;

void setUp(void) { test_integration_ctx_init(&ctx, TEST_DB); }

void tearDown(void) {
  if (cursor) {
    free(cursor);
    cursor = NULL;
  }
  if (table) {
    table = NULL;
  }
  test_integration_ctx_teardown(&ctx, TEST_DB);
}

void mock_row_free(Row row) {
  for (uint32_t i = 0; i < row.num_columns; i++) {
    if (row.values[i].type == COLUMN_TYPE_TEXT && row.values[i].string_value) {
      free(row.values[i].string_value);
    }
  }
  free(row.values);
}

void test_insert_row_with_bool_and_real(void) {
  const char *create_query =
      "CREATE TABLE items (id INT, active BOOLEAN, price REAL, name TEXT);";
  ctx.state = setup_tokenizer(create_query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  SemanticResult semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  TableResult create_result = table_create(ctx.db, ctx.node, &table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, create_result);
  TEST_ASSERT_NOT_NULL(table);

  tokenizer_free(ctx.state);
  ast_free(ctx.node);
  ctx.state = NULL;
  ctx.node = NULL;

  const char *insert_query =
      "INSERT INTO items VALUES (1, TRUE, 99.99, 'Special Item');";
  ctx.state = setup_tokenizer(insert_query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  ExecuteResult insert_result = execute_insert_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, insert_result);

  TEST_ASSERT_EQUAL(1, table->num_rows);

  cursor = table_start(table);
  TEST_ASSERT_NOT_NULL(cursor);

  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);

  TEST_ASSERT_EQUAL(1, row.id);
  TEST_ASSERT_EQUAL(true, row.values[1].bool_value);
  TEST_ASSERT_EQUAL(99.99, row.values[2].real_value);
  TEST_ASSERT_EQUAL_STRING("Special Item", row.values[3].string_value);

  mock_row_free(row);
  tokenizer_free(ctx.state);
  ast_free(ctx.node);
  ctx.state = NULL;
  ctx.node = NULL;
}

void test_insert_row(void) {
  const char *create_query = "CREATE TABLE users (id INT, name TEXT);";
  ctx.state = setup_tokenizer(create_query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  SemanticResult semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  TableResult create_result = table_create(ctx.db, ctx.node, &table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, create_result);
  TEST_ASSERT_NOT_NULL(table);

  tokenizer_free(ctx.state);
  ast_free(ctx.node);
  ctx.state = NULL;
  ctx.node = NULL;

  const char *insert_query = "INSERT INTO users VALUES (1, 'Alice');";
  ctx.state = setup_tokenizer(insert_query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  ExecuteResult insert_result = execute_insert_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, insert_result);

  TEST_ASSERT_EQUAL(1, table->num_rows);

  cursor = table_start(table);
  TEST_ASSERT_NOT_NULL(cursor);

  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);

  TEST_ASSERT_EQUAL(1, row.id);
  TEST_ASSERT_EQUAL_STRING("Alice", row.values[1].string_value);

  mock_row_free(row);
  tokenizer_free(ctx.state);
  ast_free(ctx.node);
  ctx.state = NULL;
  ctx.node = NULL;
}

void test_insert_row_multiple_columns(void) {
  const char *create_query =
      "CREATE TABLE products (id INT, name TEXT, price INT, description TEXT);";
  ctx.state = setup_tokenizer(create_query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  SemanticResult semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  TableResult create_result = table_create(ctx.db, ctx.node, &table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, create_result);
  TEST_ASSERT_NOT_NULL(table);

  tokenizer_free(ctx.state);
  ast_free(ctx.node);
  ctx.state = NULL;
  ctx.node = NULL;

  const char *insert_query = "INSERT INTO products VALUES (1, 'Laptop', 1200, "
                             "'High-end gaming laptop');";
  ctx.state = setup_tokenizer(insert_query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  ExecuteResult insert_result = execute_insert_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, insert_result);

  TEST_ASSERT_EQUAL(1, table->num_rows);

  cursor = table_start(table);
  TEST_ASSERT_NOT_NULL(cursor);

  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);

  TEST_ASSERT_EQUAL(1, row.id);
  TEST_ASSERT_EQUAL_STRING("Laptop", row.values[1].string_value);
  TEST_ASSERT_EQUAL(1200, row.values[2].int_value);
  TEST_ASSERT_EQUAL_STRING("High-end gaming laptop",
                           row.values[3].string_value);

  mock_row_free(row);
  tokenizer_free(ctx.state);
  ast_free(ctx.node);
  ctx.state = NULL;
  ctx.node = NULL;
}

int main(void) {
  UNITY_BEGIN();

  printf("\n");
  RUN_TEST(test_insert_row);
  printf("\n");
  RUN_TEST(test_insert_row_multiple_columns);
  printf("\n");
  RUN_TEST(test_insert_row_with_bool_and_real);

  return UNITY_END();
}
