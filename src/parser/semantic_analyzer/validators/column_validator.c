#include <string.h>

#include "core/table.h"
#include "parser/semantic_analyzer.h"

bool semantic_validate_columns_unique(Column *columns, int column_count,
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
