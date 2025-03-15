#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"
#include "executor/executor.h"
#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/semantic_analyzer.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"
#include "storage/cursor.h"
#include "utils/logger.h"

#include "../common/test_integration_ctx.h"
#include "core/row.h"

static const char *TEST_DB = "test_delete_row.db";
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

void test_delete_specific_row(void) {
  const char *create_query = "CREATE TABLE users (id INT, name TEXT);";
  printf("Create query: %s\n", create_query);
  ctx.state = setup_tokenizer(create_query);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);
  SemanticResult semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  Table *created_table = NULL;
  TableResult create_result = table_create(ctx.db, ctx.node, &created_table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, create_result);
  TEST_ASSERT_NOT_NULL(created_table);
  table = created_table;

  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  const char *insert_query1 = "INSERT INTO users VALUES (1, 'Alice');";
  ctx.state = setup_tokenizer(insert_query1);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);
  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  ExecuteResult exec_result = execute_insert_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, exec_result);
  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  const char *insert_query2 = "INSERT INTO users VALUES (2, 'Bob');";
  ctx.state = setup_tokenizer(insert_query2);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);
  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  exec_result = execute_insert_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, exec_result);
  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  const char *delete_query = "DELETE FROM users WHERE id = 1;";
  printf("Delete query: %s\n", delete_query);
  ctx.state = setup_tokenizer(delete_query);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_row_delete(ctx.state->tokens);
  TEST_ASSERT_NOT_NULL(ctx.node);
  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  exec_result = execute_delete_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, exec_result);
  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  TEST_ASSERT_EQUAL(1, table->num_rows);
  cursor = table_start(table);
  TEST_ASSERT_NOT_NULL(cursor);
  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);
  TEST_ASSERT_EQUAL(2, row.id);
  TEST_ASSERT_EQUAL_STRING("Bob", row.values[1].string_value);

  free_row_values(&row);
}

void test_delete_all_rows(void) {
  const char *create_query = "CREATE TABLE users (id INT, name TEXT);";
  printf("Create query: %s\n", create_query);
  ctx.state = setup_tokenizer(create_query);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);
  SemanticResult semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  Table *created_table = NULL;
  TableResult create_result = table_create(ctx.db, ctx.node, &created_table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, create_result);
  TEST_ASSERT_NOT_NULL(created_table);
  table = created_table;

  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  const char *insert_query1 = "INSERT INTO users VALUES (1, 'Alice');";
  ctx.state = setup_tokenizer(insert_query1);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);
  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  ExecuteResult exec_result = execute_insert_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, exec_result);
  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  const char *insert_query2 = "INSERT INTO users VALUES (2, 'Bob');";
  ctx.state = setup_tokenizer(insert_query2);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_row_insert(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);
  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  exec_result = execute_insert_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, exec_result);
  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  const char *delete_query = "DELETE FROM users;";
  printf("Delete all query: %s\n", delete_query);
  ctx.state = setup_tokenizer(delete_query);
  TEST_ASSERT_NOT_NULL(ctx.state);
  ctx.node = parser_row_delete(ctx.state->tokens);
  TEST_ASSERT_NOT_NULL(ctx.node);
  semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);
  ExecuteResult exec_result2 = execute_delete_rows(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, exec_result2);
  ast_free(ctx.node);
  tokenizer_free(ctx.state);
  ctx.node = NULL;
  ctx.state = NULL;

  TEST_ASSERT_EQUAL(0, table->num_rows);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_delete_specific_row);
  printf("\n");
  RUN_TEST(test_delete_all_rows);
  return UNITY_END();
}
