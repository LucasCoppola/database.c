#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/row.h"
#include "core/table.h"

#include "executor/executor.h"
#include "parser/ast.h"
#include "storage/cursor.h"
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
  Table *out_table = NULL;
  TableResult table_result = table_find(db, node, &out_table);

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", "find", table_result);
    return EXECUTE_FAILURE;
  }

  if (out_table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_FAILURE;
  }

  Row *row = malloc(sizeof(Row));
  if (!row) {
    printf("Failed to allocate row.\n");
    return EXECUTE_FAILURE;
  }

  if (strlen(node->insert_rows.values[1]) >= MAX_NAME_LENGTH) {
    printf("Name too long.\n");
    free(row);
    return EXECUTE_FAILURE;
  }
  strncpy(row->name, node->insert_rows.values[1], MAX_NAME_LENGTH);
  row->name[MAX_NAME_LENGTH - 1] = '\0';

  uint32_t id = atoi(node->insert_rows.values[0]);
  row->id = id;

  RowResult row_result = insert_row(out_table, row);
  if (row_result != ROW_SUCCESS) {
    LOG_ERROR("row", "insert", row_result);
    free(row);
    return EXECUTE_FAILURE;
  }

  free(row);
  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select_rows(Database *db, ASTNode *node) {
  Table *out_table = NULL;
  TableResult table_result = table_find(db, node, &out_table);

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", "find", table_result);
    return EXECUTE_FAILURE;
  }

  RowResult row_result = select_rows(out_table);
  if (row_result != ROW_SUCCESS) {
    LOG_ERROR("row", "select", row_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}
