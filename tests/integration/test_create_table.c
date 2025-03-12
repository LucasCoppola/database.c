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

#include "../unit/utils/test_utils.h"

// Global variables for shared test state
static const char *TEST_DB = "test_create_table.db";
Database *db = NULL;
TokenizerState *state = NULL;
ASTNode *node = NULL;

// setUp: Called before each test
void setUp(void) {
  remove(TEST_DB); // Ensure the database file is removed before each test
  DatabaseResult db_result = database_open(&db, TEST_DB);
  TEST_ASSERT_EQUAL(DATABASE_SUCCESS, db_result);
  TEST_ASSERT_NOT_NULL(db);
}

// tearDown: Called after each test
void tearDown(void) {
  if (node) {
    ast_free(node);
    node = NULL;
  }

  if (state) {
    teardown_tokenizer(state);
    state = NULL;
  }

  if (db) {
    database_close(db);
    db = NULL;
  }

  remove(TEST_DB); // Clean up the database file after each test
}

// Test: Creating table with columns
void test_create_table_with_columns(void) {
  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(state);

  node = parser_table_create(state->tokens, state->token_count);
  TEST_ASSERT_NOT_NULL(node);

  SemanticResult semantic_result = semantic_analyze(db, node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  Table *table = NULL;
  TableResult result = table_create(db, node, &table);
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
  database_close(db);
  DatabaseResult db_result = database_open(&db, TEST_DB);
  TEST_ASSERT_EQUAL(DATABASE_SUCCESS, db_result);

  ASTNode *out_node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_CREATE_TABLE, &out_node);
  TEST_ASSERT_EQUAL(AST_SUCCESS, ast_result);
  out_node->table_name = strdup("users");
  out_node->create_table.columns = NULL;
  out_node->create_table.num_columns = 0;

  Table *loaded_table = NULL;
  TableResult find_result = table_find(db, out_node->table_name, &loaded_table);
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

// Test: Attempting to create a table that already exists
void test_create_table_already_exists(void) {
  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(state);

  node = parser_table_create(state->tokens, state->token_count);
  TEST_ASSERT_NOT_NULL(node);

  SemanticResult semantic_result = semantic_analyze(db, node);
  TEST_ASSERT_EQUAL(SEMANTIC_SUCCESS, semantic_result);

  Table *table = NULL;
  TableResult result = table_create(db, node, &table);
  TEST_ASSERT_EQUAL(TABLE_SUCCESS, result);
  TEST_ASSERT_NOT_NULL(table);

  // Attempt to create the same table again
  TokenizerState *state_duplicate = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(state_duplicate);

  ASTNode *node_duplicate = parser_table_create(state_duplicate->tokens,
                                                state_duplicate->token_count);
  TEST_ASSERT_NOT_NULL(node_duplicate);

  SemanticResult semantic_result_duplicate =
      semantic_analyze(db, node_duplicate);
  TEST_ASSERT_EQUAL(SEMANTIC_DUPLICATE_TABLE, semantic_result_duplicate);

  ast_free(node_duplicate);
  teardown_tokenizer(state_duplicate);
}

// Test: Creating table with invalid column type
void test_create_table_invalid_column_type(void) {
  const char *query = "CREATE TABLE users (id FLOAT, name TEXT);";
  state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(state);

  node = parser_table_create(state->tokens, state->token_count);
  TEST_ASSERT_NULL(node); // Parser should fail for invalid column type
}

// Test: Creating table with no columns
void test_create_table_no_columns(void) {
  const char *query = "CREATE TABLE empty ();";
  state = setup_tokenizer(query);
  TEST_ASSERT_NOT_NULL(state);

  node = parser_table_create(state->tokens, state->token_count);
  TEST_ASSERT_NULL(node); // Parser should fail for empty columns list
}

// Main function to run all tests
int main(void) {
  UNITY_BEGIN();

  // Register test cases
  RUN_TEST(test_create_table_with_columns);
  RUN_TEST(test_create_table_already_exists);
  RUN_TEST(test_create_table_invalid_column_type);
  RUN_TEST(test_create_table_no_columns);

  return UNITY_END();
}
