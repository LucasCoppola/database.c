#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"
#include "libs/unity.h"
#include "parser/ast.h"
#include "parser/semantic_analyzer.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "../common/test_integration_ctx.h"

static const char *TEST_DB = "test_create_table.db";
static TestIntegrationCtx ctx;

void setUp(void) { test_integration_ctx_init(&ctx, TEST_DB); }

void tearDown(void) { test_integration_ctx_teardown(&ctx, TEST_DB); }

void test_create_table_with_columns(void) {
  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  printf("Query: %s\n", query);
  ctx.state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  SemanticResult semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  Table *table = NULL;
  TableResult result = table_create(ctx.db, ctx.node, &table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, result);
  TEST_ASSERT_NOT_NULL(table);

  TEST_ASSERT_EQUAL_STRING("users", table->name);
  TEST_ASSERT_EQUAL(2, table->num_columns);
  TEST_ASSERT_EQUAL(0, table->num_rows);

  TEST_ASSERT_EQUAL_STRING("id", table->columns[0].name);
  TEST_ASSERT_EQUAL(COLUMN_TYPE_INT, table->columns[0].type);
  TEST_ASSERT_EQUAL_STRING("name", table->columns[1].name);
  TEST_ASSERT_EQUAL(COLUMN_TYPE_TEXT, table->columns[1].type);

  // Reopen the database to verify persistence
  database_close(ctx.db);
  DatabaseResult db_result = database_open(&ctx.db, TEST_DB);
  TEST_ASSERT_EQUAL(DATABASE_SUCCESS, db_result);

  ASTNode *out_node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_CREATE_TABLE, &out_node);
  TEST_ASSERT_EQUAL(AST_SUCCESS, ast_result);
  out_node->table_name = strdup("users");
  out_node->create_table.columns = NULL;
  out_node->create_table.num_columns = 0;

  Table *loaded_table = NULL;
  TableResult find_result =
      table_find(ctx.db, out_node->table_name, &loaded_table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, find_result);
  TEST_ASSERT_NOT_NULL(loaded_table);

  TEST_ASSERT_EQUAL_STRING("users", loaded_table->name);
  TEST_ASSERT_EQUAL(2, loaded_table->num_columns);
  TEST_ASSERT_EQUAL(0, loaded_table->num_rows);

  TEST_ASSERT_EQUAL_STRING("id", loaded_table->columns[0].name);
  TEST_ASSERT_EQUAL(COLUMN_TYPE_INT, loaded_table->columns[0].type);
  TEST_ASSERT_EQUAL_STRING("name", loaded_table->columns[1].name);
  TEST_ASSERT_EQUAL(COLUMN_TYPE_TEXT, loaded_table->columns[1].type);

  ast_free(out_node);
}

void test_create_table_already_exists(void) {
  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  printf("Query: %s\n", query);
  ctx.state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NOT_NULL(ctx.node);

  SemanticResult semantic_result = semantic_analyze(ctx.db, ctx.node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  Table *table = NULL;
  TableResult result = table_create(ctx.db, ctx.node, &table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, result);
  TEST_ASSERT_NOT_NULL(table);

  // Attempt to create the same table again
  TokenizerState *state_duplicate = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(state_duplicate);

  ASTNode *node_duplicate = parser_table_create(state_duplicate->tokens,
                                                state_duplicate->token_count);
  TEST_ASSERT_NOT_NULL(node_duplicate);

  SemanticResult semantic_result_duplicate =
      semantic_analyze(ctx.db, node_duplicate);
  TEST_ASSERT_EQUAL(SEMANTIC_DUPLICATE_TABLE, semantic_result_duplicate);

  ast_free(node_duplicate);
  tokenizer_free(state_duplicate);
}

void test_create_table_invalid_column_type(void) {
  const char *query = "CREATE TABLE users (id FLOAT, name TEXT);";
  printf("Query: %s\n", query);
  ctx.state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NULL(ctx.node);
}

void test_create_table_no_columns(void) {
  const char *query = "CREATE TABLE empty ();";
  printf("Query: %s\n", query);
  ctx.state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(ctx.state);

  ctx.node = parser_table_create(ctx.state->tokens, ctx.state->token_count);
  TEST_ASSERT_NULL(ctx.node);
}

int main(void) {
  UNITY_BEGIN();

  printf("\n");
  RUN_TEST(test_create_table_with_columns);
  printf("\n");
  RUN_TEST(test_create_table_already_exists);
  printf("\n");
  RUN_TEST(test_create_table_invalid_column_type);
  printf("\n");
  RUN_TEST(test_create_table_no_columns);

  return UNITY_END();
}
