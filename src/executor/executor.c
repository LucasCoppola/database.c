#include <stdbool.h>
#include <stdio.h>

#include "core/database.h"
#include "core/table.h"
#include "executor/executor.h"
#include "parser/ast.h"
#include "utils/logger.h"

#include "core/row.h"

ExecuteResult execute_ast(Database *db, ASTNode *node) {
  if (!node) {
    fprintf(stderr, "Error: Null AST node\n");
    return EXECUTE_FAILURE;
  }

  switch (node->type) {
  case NODE_CREATE_TABLE:
    return execute_create_table(db, node);
  case NODE_DROP_TABLE:
    return execute_drop_table(db, node);
  case NODE_INSERT:
    return execute_insert_rows(db, node);
  case NODE_SELECT:
    return execute_select_rows(db, node);
  default:
    fprintf(stderr, "Error: Unknown node type %d\n", node->type);
    return EXECUTE_FAILURE;
  }
}

ExecuteResult execute_create_table(Database *db, ASTNode *node) {
  Table *out_table = NULL;
  TableResult table_result = table_create(db, node, &out_table);
  if (table_result != TABLE_SUCCESS) {
    DEBUG_LOG("table", "create", table_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_drop_table(Database *db, ASTNode *node) {
  TableResult table_result = table_drop(db, node->table_name);
  if (table_result != TABLE_SUCCESS) {
    DEBUG_LOG("table", "drop", table_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_insert_rows(Database *db, ASTNode *node) {
  Table *table = NULL;
  TableResult table_result = table_find(db, node->table_name, &table);

  if (table_result != TABLE_SUCCESS) {
    DEBUG_LOG("table", "find", table_result);
    return EXECUTE_FAILURE;
  }

  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_FAILURE;
  }

  RowResult row_result = insert_row(table, node);
  if (row_result != ROW_SUCCESS) {
    DEBUG_LOG("row", "insert", row_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select_rows(Database *db, ASTNode *node) {
  Table *out_table = NULL;
  TableResult table_result = table_find(db, node->table_name, &out_table);

  if (table_result != TABLE_SUCCESS) {
    DEBUG_LOG("table", "find", table_result);
    return EXECUTE_FAILURE;
  }

  RowResult row_result = select_row(out_table, node);
  if (row_result != ROW_SUCCESS) {
    DEBUG_LOG("row", "select", row_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}
