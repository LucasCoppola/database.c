#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"

#include "core/row.h"
#include "executor/executor.h"
#include "parser/ast.h"
#include "utils/logger.h"

ExecuteResult execute_create_table(Database *db, ASTNode *node) {
  Table *out_table = NULL;
  TableResult table_result = table_create(db, node, &out_table);
  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", "create", table_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_drop_table(Database *db, ASTNode *node) {
  TableResult table_result = table_drop(db, node);
  if (table_result == TABLE_NOT_FOUND) {
    printf("Table not found.\n");
    return EXECUTE_FAILURE;
  }

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", "drop", table_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_insert_rows(Database *db, ASTNode *node) {
  Table *table = NULL;
  TableResult table_result = table_find(db, node, &table);

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", "find", table_result);
    return EXECUTE_FAILURE;
  }

  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_FAILURE;
  }

  RowResult row_result = insert_row(table, node);
  if (row_result != ROW_SUCCESS) {
    LOG_ERROR("row", "insert", row_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select_rows(Database *db, ASTNode *node) {
  Table *out_table = NULL;
  TableResult table_result = table_find(db, node, &out_table);

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", "find", table_result);
    return EXECUTE_FAILURE;
  }

  RowResult row_result = select_row(out_table, node);
  if (row_result != ROW_SUCCESS) {
    LOG_ERROR("row", "select", row_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}
