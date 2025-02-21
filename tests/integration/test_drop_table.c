#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"
#include "executor/executor.h"
#include "parser/ast.h"
#include "parser/statements.h"
#include "parser/tokenizer.h"

#include "../unit/utils/test_utils.h"

static const char *TEST_DB = "test_drop_table.db";

void test_drop_table_success() {
  printf("Test: Dropping an existing table\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *create_query = "CREATE TABLE users (id INT, name TEXT);";
  TokenizerState *create_state = setup_tokenizer(create_query);
  ASTNode *create_node =
      parser_table_create(create_state->tokens, create_state->token_count);
  assert(create_node != NULL && "Failed to parse CREATE TABLE statement");
  Table *table = NULL;
  TableResult create_result = table_create(db, create_node, &table);
  assert(create_result == TABLE_SUCCESS && "Failed to create table");
  assert(table != NULL && "Table pointer is NULL");

  const char *drop_query = "DROP TABLE users;";
  TokenizerState *drop_state = setup_tokenizer(drop_query);
  ASTNode *drop_node = parser_table_drop(drop_state->tokens);
  assert(drop_node != NULL && "Failed to parse DROP TABLE statement");
  ExecuteResult exec_result = execute_drop_table(db, drop_node);
  assert(exec_result == EXECUTE_SUCCESS && "Failed to execute DROP TABLE");

  ASTNode *out_node = NULL;
  ASTNodeResult ast_result = create_ast_node(NODE_DROP_TABLE, &out_node);
  assert(ast_result == AST_SUCCESS && "Failed to create AST Node");
  out_node->table_name = strdup("users");

  Table *dropped_table = NULL;
  TableResult find_result = table_find(db, out_node, &dropped_table);
  assert(find_result == TABLE_NOT_FOUND &&
         "Table should not be found after drop");

  ast_free(create_node);
  ast_free(drop_node);
  ast_free(out_node);
  teardown_tokenizer(create_state);
  teardown_tokenizer(drop_state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Drop table test passed\n");
}

void test_drop_table_nonexistent() {
  printf("Test: Dropping a non-existent table\n");

  remove(TEST_DB);
  Database *db = NULL;
  DatabaseResult db_result = database_open(&db, TEST_DB);
  assert(db_result == DATABASE_SUCCESS && "Failed to open database");
  assert(db != NULL && "Database pointer is NULL");

  const char *drop_query = "DROP TABLE nonexistent_table;";
  TokenizerState *drop_state = setup_tokenizer(drop_query);
  ASTNode *drop_node = parser_table_drop(drop_state->tokens);
  assert(drop_node != NULL && "Failed to parse DROP TABLE statement");
  ExecuteResult exec_result = execute_drop_table(db, drop_node);
  assert(exec_result == EXECUTE_FAILURE &&
         "Dropping a non-existent table should fail");

  ast_free(drop_node);
  teardown_tokenizer(drop_state);
  database_close(db);
  remove(TEST_DB);

  printf("✓ Drop non-existent table test passed\n");
}

int main() {
  printf("Running drop table integration tests...\n\n");

  test_drop_table_success();
  test_drop_table_nonexistent();

  printf("\nAll drop table integration tests passed!\n");
  return 0;
}
