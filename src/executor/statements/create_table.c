#include <stdbool.h>
#include <stdio.h>

#include "core/database.h"
#include "core/table.h"

#include "executor/executor.h"
#include "parser/ast.h"
#include "utils/logger.h"

ExecuteResult execute_create_table(Database *db, ASTNode *node) {
  char *table_name = node->table_name;
  // Column *columns = node->create_table.columns;
  // int num_columns = node->create_table.num_columns;

  Table *out_table = NULL;
  TableResult table_result = table_create(db, table_name, &out_table);
  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", table_result);
    return EXECUTE_FAILURE;
  }

  return EXECUTE_SUCCESS;
}
