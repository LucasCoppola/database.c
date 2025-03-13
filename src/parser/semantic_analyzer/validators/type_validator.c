#include "parser/semantic_analyzer.h"
#include "utils/convertions.h"

SemanticResult semantic_validate_data_types(Column *columns, int num_columns,
                                            Value **values,
                                            char **expected_type,
                                            char **found_type,
                                            char **column_name) {
  for (int i = 0; i < num_columns; i++) {
    if (columns[i].type != values[i]->type) {
      *expected_type = data_type_to_string(columns[i].type);
      *found_type = data_type_to_string(values[i]->type);
      *column_name = columns[i].name;
      return SEMANTIC_TYPE_MISMATCH;
    }
  }
  return SEMANTIC_SUCCESS;
}
