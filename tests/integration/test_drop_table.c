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

#include "../common/test_integration_ctx.h"

static const char *TEST_DB = "test_drop_table.db";
static TestIntegrationCtx ctx;

void setUp(void) { test_integration_ctx_init(&ctx, TEST_DB); }

void tearDown(void) { test_integration_ctx_teardown(&ctx, TEST_DB); }

void test_drop_table_success(void) {
  const char *create_query = "CREATE TABLE users (id INT, name TEXT);";
  printf("Query: %s\n", create_query);
  TokenizerState *state = setup_tokenizer(create_query);
  TEST_ASSERT_NOT_NULL(state);

  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  TEST_ASSERT_NOT_NULL(node);

  SemanticResult semantic_result = semantic_analyze(ctx.db, node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  Table *table = NULL;
  TableResult create_result = table_create(ctx.db, node, &table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, create_result);
  TEST_ASSERT_NOT_NULL(table);

  if (state) {
    tokenizer_free(state);
    state = NULL;
  }
  if (node) {
    ast_free(node);
    node = NULL;
  }

  const char *drop_query = "DROP TABLE users;";
  state = setup_tokenizer(drop_query);
  TEST_ASSERT_NOT_NULL(state);

  node = parser_table_drop(state->tokens);
  TEST_ASSERT_NOT_NULL(node);

  semantic_result = semantic_analyze(ctx.db, node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  ExecuteResult exec_result = execute_drop_table(ctx.db, node);
  TEST_ASSERT_EQUAL(EXECUTE_SUCCESS, exec_result);

  // Verify the table no longer exists
  ASTNode *out_node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_DROP_TABLE, &out_node);
  TEST_ASSERT_EQUAL(AST_SUCCESS, ast_result);
  out_node->table_name = strdup("users");

  Table *dropped_table = NULL;
  TableResult find_result =
      table_find(ctx.db, out_node->table_name, &dropped_table);
  TEST_ASSERT_EQUAL(TABLE_NOT_FOUND, find_result);

  ast_free(out_node);
}

void test_drop_table_nonexistent(void) {
  const char *drop_query = "DROP TABLE nonexistent_table;";
  printf("Query: %s\n", drop_query);
  TokenizerState *state = setup_tokenizer(drop_query);
  TEST_ASSERT_NOT_NULL(state);

  ASTNode *node = parser_table_drop(state->tokens);
  TEST_ASSERT_NOT_NULL(node);

  SemanticResult semantic_result = semantic_analyze(ctx.db, node);
  TEST_ASSERT_EQUAL(SEMANTIC_TABLE_NOT_FOUND, semantic_result);
}

int main(void) {
  UNITY_BEGIN();

  printf("\n");
  RUN_TEST(test_drop_table_success);
  printf("\n");
  RUN_TEST(test_drop_table_nonexistent);

  return UNITY_END();
}
