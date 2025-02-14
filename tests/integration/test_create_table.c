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

void test_create_table_with_columns(void) {
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

int main(void) {
  printf("Running create table integration tests...\n\n");

  test_create_table_with_columns();

  printf("\nAll create table integration tests passed!\n");
  return 0;
}
