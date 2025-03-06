#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"
#include "parser/ast.h"
#include "parser/semantic_analyzer.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "../unit/utils/test_utils.h"

static const char *TEST_DB = "test_create_table.db";

void test_create_table_with_columns() {
  printf("Test: Creating table with columns\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node != NULL && "Failed to parse CREATE TABLE statement");

  SemanticResult semantic_result = semantic_analyze(db, node);
  assert(semantic_result == SEMANTIC_SUCCESS &&
         "Semantic analysis should pass");

  Table *table = NULL;
  TableResult result = table_create(db, node, &table);
  assert(result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  assert(strcmp(table->name, "users") == 0 && "Table name mismatch");
  assert(table->num_columns == 2 && "Wrong number of columns");
  assert(table->num_rows == 0 && "Table should be empty");

  assert(strcmp(table->columns[0].name, "id") == 0 &&
         "First column name mismatch");
  assert(table->columns[0].type == COLUMN_TYPE_INT &&
         "First column type mismatch");
  assert(strcmp(table->columns[1].name, "name") == 0 &&
         "Second column name mismatch");
  assert(table->columns[1].type == COLUMN_TYPE_TEXT &&
         "Second column type mismatch");

  database_close(db);
  db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to reopen database");

  ASTNode *out_node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_CREATE_TABLE, &out_node);
  assert(ast_result == AST_SUCCESS && "Failed to create AST Node");
  out_node->table_name = strdup("users");
  out_node->create_table.columns = NULL;
  out_node->create_table.num_columns = 0;

  Table *loaded_table = NULL;
  TableResult find_result = table_find(db, out_node->table_name, &loaded_table);
  assert(find_result == TABLE_SUCCESS && "Failed to find table after reload");
  assert(loaded_table != NULL && "Loaded table pointer is NULL");

  assert(strcmp(loaded_table->name, "users") == 0 &&
         "Loaded table name mismatch");
  assert(loaded_table->num_columns == 2 &&
         "Wrong number of columns after reload");
  assert(loaded_table->num_rows == 0 && "Loaded table should be empty");

  assert(strcmp(loaded_table->columns[0].name, "id") == 0 &&
         "Loaded first column name mismatch");
  assert(loaded_table->columns[0].type == COLUMN_TYPE_INT &&
         "Loaded first column type mismatch");
  assert(strcmp(loaded_table->columns[1].name, "name") == 0 &&
         "Loaded second column name mismatch");
  assert(loaded_table->columns[1].type == COLUMN_TYPE_TEXT &&
         "Loaded second column type mismatch");

  ast_free(node);
  ast_free(out_node);
  teardown_tokenizer(state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Create table test passed\n");
}

void test_create_table_already_exists() {
  printf("Test: Attempting to create a table that already exists\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node != NULL && "Failed to parse CREATE TABLE statement");

  SemanticResult semantic_result = semantic_analyze(db, node);
  assert(semantic_result == SEMANTIC_SUCCESS &&
         "Semantic analysis should pass");

  Table *table = NULL;
  TableResult result = table_create(db, node, &table);
  assert(result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  TokenizerState *state_duplicate = setup_tokenizer(query);
  assert(state_duplicate != NULL &&
         "Failed to setup tokenizer for duplicate table");

  ASTNode *node_duplicate = parser_table_create(state_duplicate->tokens,
                                                state_duplicate->token_count);
  assert(node_duplicate != NULL &&
         "Failed to parse duplicate CREATE TABLE statement");

  SemanticResult semantic_result_duplicate =
      semantic_analyze(db, node_duplicate);
  assert(semantic_result_duplicate == SEMANTIC_DUPLICATE_TABLE &&
         "Semantic analyzer should detect duplicate table");

  ast_free(node);
  ast_free(node_duplicate);
  teardown_tokenizer(state);
  teardown_tokenizer(state_duplicate);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Create table already exists test passed\n");
}

void test_create_table_invalid_column_type() {
  printf("Test: Creating table with invalid column type\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *query = "CREATE TABLE users (id FLOAT, name TEXT);";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node == NULL && "Parser should fail for invalid column type");

  teardown_tokenizer(state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Create table with invalid column type test passed\n");
}

void test_create_table_no_columns() {
  printf("Test: Creating table with no columns\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *query = "CREATE TABLE empty ();";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node == NULL && "Parser should fail for empty columns list");

  teardown_tokenizer(state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Create table with no columns test passed\n");
}

int main() {
  printf("Running create table integration tests...\n\n");

  test_create_table_with_columns();
  test_create_table_already_exists();
  test_create_table_invalid_column_type();
  test_create_table_no_columns();

  printf("\nAll create table integration tests passed!\n");
  return 0;
}
