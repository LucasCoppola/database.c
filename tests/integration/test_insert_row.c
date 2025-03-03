#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"

#include "parser/ast.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "core/row.h"
#include "executor/executor.h"
#include "storage/cursor.h"
#include "utils/logger.h"

#include "../unit/utils/test_utils.h"

static const char *TEST_DB = "test_insert_row.db";

void test_row_free(Row row) {
  for (uint32_t i = 0; i < row.num_columns; i++) {
    if (row.values[i].type == COLUMN_TYPE_TEXT) {
      free(row.values[i].string_value);
    }
  }
  free(row.values);
}

void test_insert_row() {
  printf("Test: Inserting a row into a table\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *create_query = "CREATE TABLE users (id INT, name TEXT);";
  TokenizerState *create_state = setup_tokenizer(create_query);
  assert(create_state != NULL && "Failed to setup tokenizer for CREATE TABLE");

  ASTNode *create_node =
      parser_table_create(create_state->tokens, create_state->token_count);
  assert(create_node != NULL && "Failed to parse CREATE TABLE statement");

  Table *table = NULL;
  TableResult create_result = table_create(db, create_node, &table);
  assert(create_result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  const char *insert_query = "INSERT INTO users VALUES (1, 'Alice');";
  TokenizerState *insert_state = setup_tokenizer(insert_query);
  assert(insert_state != NULL && "Failed to setup tokenizer for INSERT");

  ASTNode *insert_node =
      parser_row_insert(insert_state->tokens, insert_state->token_count);
  assert(insert_node != NULL && "Failed to parse INSERT statement");

  // Execute the INSERT statement
  ExecuteResult insert_result = execute_insert_rows(db, insert_node);
  assert(insert_result == EXECUTE_SUCCESS && "Failed to insert row");

  assert(table->num_rows == 1 && "Table should have one row");

  Cursor *cursor = table_start(table);
  assert(cursor != NULL && "Failed to create cursor");

  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);

  assert(row.id == 1 && "Row ID mismatch");
  assert(strcmp(row.values[1].string_value, "Alice") == 0 &&
         "Row name mismatch");

  database_close(db);
  db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to reopen database");

  ASTNode *out_node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_INSERT, &out_node);
  assert(ast_result == AST_SUCCESS && "Failed to create AST Node");
  out_node->table_name = strdup("users");
  out_node->insert_rows.values = NULL;
  out_node->insert_rows.num_values = 0;

  Table *loaded_table = NULL;
  TableResult find_result = table_find(db, out_node->table_name, &loaded_table);
  assert(find_result == TABLE_SUCCESS && "Failed to find table after reload");
  assert(loaded_table != NULL && "Loaded table pointer is NULL");

  assert(loaded_table->num_rows == 1 && "Loaded table should have one row");

  Cursor *loaded_cursor = table_start(loaded_table);
  assert(loaded_cursor != NULL && "Failed to create cursor for loaded table");

  Row loaded_row;
  void *loaded_row_location = cursor_value(loaded_cursor);
  deserialize_row(loaded_row_location, &loaded_row, loaded_table);

  assert(loaded_row.id == 1 && "Loaded row ID mismatch");
  assert(strcmp(loaded_row.values[1].string_value, "Alice") == 0 &&
         "Loaded row name mismatch");

  test_row_free(loaded_row);
  test_row_free(row);
  ast_free(create_node);
  ast_free(insert_node);
  ast_free(out_node);
  teardown_tokenizer(create_state);
  teardown_tokenizer(insert_state);
  free(cursor);
  free(loaded_cursor);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Insert row test passed\n");
}

void test_insert_row_multiple_columns() {
  printf("Test: Inserting a row into a table with multiple columns\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *create_query =
      "CREATE TABLE products (id INT, name TEXT, price INT, description TEXT);";
  TokenizerState *create_state = setup_tokenizer(create_query);
  assert(create_state != NULL && "Failed to setup tokenizer for CREATE TABLE");

  ASTNode *create_node =
      parser_table_create(create_state->tokens, create_state->token_count);
  assert(create_node != NULL && "Failed to parse CREATE TABLE statement");

  Table *table = NULL;
  TableResult create_result = table_create(db, create_node, &table);
  assert(create_result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  const char *insert_query = "INSERT INTO products VALUES (1, 'Laptop', 1200, "
                             "'High-end gaming laptop');";
  TokenizerState *insert_state = setup_tokenizer(insert_query);
  assert(insert_state != NULL && "Failed to setup tokenizer for INSERT");

  ASTNode *insert_node =
      parser_row_insert(insert_state->tokens, insert_state->token_count);
  assert(insert_node != NULL && "Failed to parse INSERT statement");

  // Execute the INSERT statement
  ExecuteResult insert_result = execute_insert_rows(db, insert_node);
  assert(insert_result == EXECUTE_SUCCESS && "Failed to insert row");

  assert(table->num_rows == 1 && "Table should have one row");

  Cursor *cursor = table_start(table);
  assert(cursor != NULL && "Failed to create cursor");

  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);

  assert(row.id == 1 && "Row ID mismatch");
  assert(strcmp(row.values[1].string_value, "Laptop") == 0 &&
         "Row name mismatch");
  assert(row.values[2].int_value == 1200 && "Row price mismatch");
  assert(strcmp(row.values[3].string_value, "High-end gaming laptop") == 0 &&
         "Row description mismatch");

  test_row_free(row);
  ast_free(create_node);
  ast_free(insert_node);
  teardown_tokenizer(create_state);
  teardown_tokenizer(insert_state);
  free(cursor);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Insert row with multiple columns test passed\n");
}

void test_insert_row_int_only() {
  printf("Test: Inserting a row into a table with only INT columns\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *create_query =
      "CREATE TABLE scores (player_id INT, score INT, level INT);";
  TokenizerState *create_state = setup_tokenizer(create_query);
  assert(create_state != NULL && "Failed to setup tokenizer for CREATE TABLE");

  ASTNode *create_node =
      parser_table_create(create_state->tokens, create_state->token_count);
  assert(create_node != NULL && "Failed to parse CREATE TABLE statement");

  Table *table = NULL;
  TableResult create_result = table_create(db, create_node, &table);
  assert(create_result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  const char *insert_query = "INSERT INTO scores VALUES (1, 100, 5);";
  TokenizerState *insert_state = setup_tokenizer(insert_query);
  assert(insert_state != NULL && "Failed to setup tokenizer for INSERT");

  ASTNode *insert_node =
      parser_row_insert(insert_state->tokens, insert_state->token_count);
  assert(insert_node != NULL && "Failed to parse INSERT statement");

  // Execute the INSERT statement
  ExecuteResult insert_result = execute_insert_rows(db, insert_node);
  assert(insert_result == EXECUTE_SUCCESS && "Failed to insert row");

  assert(table->num_rows == 1 && "Table should have one row");

  Cursor *cursor = table_start(table);
  assert(cursor != NULL && "Failed to create cursor");

  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);

  assert(row.id == 1 && "Row ID mismatch");
  assert(row.values[1].int_value == 100 && "Row score mismatch");
  assert(row.values[2].int_value == 5 && "Row level mismatch");

  test_row_free(row);
  ast_free(create_node);
  ast_free(insert_node);
  teardown_tokenizer(create_state);
  teardown_tokenizer(insert_state);
  free(cursor);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Insert row with only INT columns test passed\n");
}

void test_insert_row_text_only() {
  printf("Test: Inserting a row into a table with only TEXT columns\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *create_query =
      "CREATE TABLE messages (sender TEXT, receiver TEXT, message TEXT);";
  TokenizerState *create_state = setup_tokenizer(create_query);
  assert(create_state != NULL && "Failed to setup tokenizer for CREATE TABLE");

  ASTNode *create_node =
      parser_table_create(create_state->tokens, create_state->token_count);
  assert(create_node != NULL && "Failed to parse CREATE TABLE statement");

  Table *table = NULL;
  TableResult create_result = table_create(db, create_node, &table);
  assert(create_result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  const char *insert_query =
      "INSERT INTO messages VALUES ('Alice', 'Bob', 'Hello, Bob!');";
  TokenizerState *insert_state = setup_tokenizer(insert_query);
  assert(insert_state != NULL && "Failed to setup tokenizer for INSERT");

  ASTNode *insert_node =
      parser_row_insert(insert_state->tokens, insert_state->token_count);
  assert(insert_node != NULL && "Failed to parse INSERT statement");

  // Execute the INSERT statement
  ExecuteResult insert_result = execute_insert_rows(db, insert_node);
  assert(insert_result == EXECUTE_SUCCESS && "Failed to insert row");

  assert(table->num_rows == 1 && "Table should have one row");

  Cursor *cursor = table_start(table);
  assert(cursor != NULL && "Failed to create cursor");

  Row row;
  void *row_location = cursor_value(cursor);
  deserialize_row(row_location, &row, table);

  assert(strcmp(row.values[0].string_value, "Alice") == 0 &&
         "Row sender mismatch");
  assert(strcmp(row.values[1].string_value, "Bob") == 0 &&
         "Row receiver mismatch");
  assert(strcmp(row.values[2].string_value, "Hello, Bob!") == 0 &&
         "Row message mismatch");

  test_row_free(row);
  ast_free(create_node);
  ast_free(insert_node);
  teardown_tokenizer(create_state);
  teardown_tokenizer(insert_state);
  free(cursor);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Insert row with only TEXT columns test passed\n");
}

int main() {
  printf("Running insert row integration tests...\n\n");

  test_insert_row();
  printf("\n");
  test_insert_row_multiple_columns();
  printf("\n");
  test_insert_row_int_only();
  printf("\n");
  test_insert_row_text_only();

  printf("\nAll insert row integration tests passed!\n");
  return 0;
}
