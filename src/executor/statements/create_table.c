#include <stdbool.h>
#include <stdio.h>

#include "core/database.h"
#include "core/table.h"

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
  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", "drop", table_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}
