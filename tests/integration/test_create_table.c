#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"
#include "parser/ast.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "../unit/utils/test_utils.h"

static const char *TEST_DB = "test_create_table.db";

void test_create_table_with_columns() {
  printf("Test: Creating table with columns\n");

  // Setup
  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  // Create a table with columns
  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  // Parse the CREATE TABLE statement
  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node != NULL && "Failed to parse CREATE TABLE statement");

  // Execute the CREATE TABLE statement
  Table *table = NULL;
  TableResult result = table_create(db, node, &table);
  assert(result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  // Verify table properties
  assert(strcmp(table->name, "users") == 0 && "Table name mismatch");
  assert(table->num_columns == 2 && "Wrong number of columns");
  assert(table->num_rows == 0 && "Table should be empty");

  // Verify column properties
  assert(strcmp(table->columns[0].name, "id") == 0 &&
         "First column name mismatch");
  assert(table->columns[0].type == TYPE_INT && "First column type mismatch");
  assert(strcmp(table->columns[1].name, "name") == 0 &&
         "Second column name mismatch");
  assert(table->columns[1].type == TYPE_TEXT && "Second column type mismatch");

  // Test persistence by closing and reopening
  database_close(db);
  db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to reopen database");

  // Find the table after reopening
  Table *loaded_table = NULL;
  TableResult find_result = table_find(db, "users", &loaded_table);
  assert(find_result == TABLE_SUCCESS && "Failed to find table after reload");
  assert(loaded_table != NULL && "Loaded table pointer is NULL");

  // Verify loaded table properties
  assert(strcmp(loaded_table->name, "users") == 0 &&
         "Loaded table name mismatch");
  assert(loaded_table->num_columns == 2 &&
         "Wrong number of columns after reload");
  assert(loaded_table->num_rows == 0 && "Loaded table should be empty");

  // Verify loaded column properties
  assert(strcmp(loaded_table->columns[0].name, "id") == 0 &&
         "Loaded first column name mismatch");
  assert(loaded_table->columns[0].type == TYPE_INT &&
         "Loaded first column type mismatch");
  assert(strcmp(loaded_table->columns[1].name, "name") == 0 &&
         "Loaded second column name mismatch");
  assert(loaded_table->columns[1].type == TYPE_TEXT &&
         "Loaded second column type mismatch");

  // Cleanup
  ast_free(node);
  teardown_tokenizer(state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Create table test passed\n");
}

void test_create_table_already_exists() {
  printf("Test: Attempting to create a table that already exists\n");

  // Setup
  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  // Create a table with columns
  const char *query = "CREATE TABLE users (id INT, name TEXT);";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  // Parse the CREATE TABLE statement
  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node != NULL && "Failed to parse CREATE TABLE statement");

  // Execute the CREATE TABLE statement
  Table *table = NULL;
  TableResult result = table_create(db, node, &table);
  assert(result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  // Attempt to create the same table again
  TokenizerState *state_duplicate = setup_tokenizer(query);
  assert(state_duplicate != NULL &&
         "Failed to setup tokenizer for duplicate table");

  ASTNode *node_duplicate = parser_table_create(state_duplicate->tokens,
                                                state_duplicate->token_count);
  assert(node_duplicate != NULL &&
         "Failed to parse duplicate CREATE TABLE statement");

  Table *table_duplicate = NULL;
  TableResult result_duplicate =
      table_create(db, node_duplicate, &table_duplicate);

  // Verify that the second attempt to create the table fails
  assert(result_duplicate == TABLE_ALREADY_EXISTS &&
         "Expected TABLE_ERROR_ALREADY_EXISTS when creating a duplicate table");
  assert(table_duplicate == NULL &&
         "Table pointer should be NULL for duplicate table creation");

  // Cleanup
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

  // Setup
  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  // Create a table with invalid column type
  const char *query = "CREATE TABLE users (id FLOAT, name TEXT);";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  // Parse the CREATE TABLE statement
  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node == NULL && "Parser should fail for invalid column type");

  // Cleanup
  teardown_tokenizer(state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Create table with invalid column type test passed\n");
}

void test_create_table_max_name_length() {
  printf("Test: Creating table with maximum name length\n");

  // Setup
  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  // Create a table with a very long name
  char long_name[MAX_NAME_LENGTH * 2];
  memset(long_name, 'a', MAX_NAME_LENGTH + 10);
  long_name[MAX_NAME_LENGTH + 10] = '\0';

  char query[MAX_NAME_LENGTH * 3];
  snprintf(query, sizeof(query), "CREATE TABLE %s (id INT);", long_name);

  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  // Parse the CREATE TABLE statement
  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node != NULL && "Failed to parse CREATE TABLE statement");

  // Execute the CREATE TABLE statement - should fail
  Table *table = NULL;
  TableResult result = table_create(db, node, &table);
  assert(result == TABLE_NAME_TOO_LONG &&
         "Should fail for too long table name");
  assert(table == NULL && "Table should not be created");

  // Cleanup
  ast_free(node);
  teardown_tokenizer(state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Create table with max name length test passed\n");
}

void test_create_table_no_columns() {
  printf("Test: Creating table with no columns\n");

  // Setup
  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  // Try to create a table with no columns
  const char *query = "CREATE TABLE empty ();";
  TokenizerState *state = setup_tokenizer(query);
  assert(state != NULL && "Failed to setup tokenizer");

  // Parse the CREATE TABLE statement
  ASTNode *node = parser_table_create(state->tokens, state->token_count);
  assert(node == NULL && "Parser should fail for empty columns list");

  // Cleanup
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
  test_create_table_max_name_length();
  test_create_table_no_columns();

  printf("\nAll create table integration tests passed!\n");
  return 0;
}
