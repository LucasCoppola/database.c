#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core/table.h"
#include "parser/ast.h"
#include "parser/semantic_analyzer.h"

bool semantic_validate_columns_uniqueness(Column *columns, int column_count,
                                          char **out_column) {
  for (int i = 0; i < column_count; i++) {
    for (int j = i + 1; j < column_count; j++) {
      if (strcmp(columns[i].name, columns[j].name) == 0) {
        *out_column = columns[i].name;
        return false;
      }
    }
  }

  return true;
}

bool semantic_validate_select_columns(Table *table, ASTNode *node,
                                      char **out_column) {
  for (int i = 0; i < node->select_rows.num_columns; i++) {
    char *column_name = node->select_rows.select_columns[i];
    bool column_found = false;

    for (uint32_t j = 0; j < table->num_columns; j++) {
      if (strcmp(column_name, table->columns[j].name) == 0) {
        column_found = true;
        break;
      }
    }

    if (!column_found) {
      *out_column = column_name;
      return false;
    }
  }

  return true;
}

void convert_uint32_to_str(uint32_t num, char *str, size_t str_size) {
  snprintf(str, str_size, "%" PRIu32, num);
}
