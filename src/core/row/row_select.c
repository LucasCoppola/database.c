#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "core/table.h"

#include "core/hashmap.h"
#include "core/row.h"
#include "parser/ast.h"
#include "storage/cursor.h"
#include "storage/pager.h"
#include "utils/logger.h"

RowResult select_row(Table *table, ASTNode *node) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  Cursor *cursor = table_start(table);
  Row row;
  row.values = NULL;

  while (!cursor->end_of_table) {
    deserialize_row(cursor_value(cursor), &row, table);

    bool should_print = true;
    if (has_where_condition(node)) {
      should_print =
          evaluate_where_condition(&row, &node->where_condition, table);
    }
    if (should_print) {
      print_row(row, node, table);
    }

    cursor_advance(cursor);
  }

  free_row_values(&row);
  free(cursor);
  return ROW_SUCCESS;
}
