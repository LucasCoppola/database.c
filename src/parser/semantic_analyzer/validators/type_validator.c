#include "parser/semantic_analyzer.h"

char *data_type_to_string(DataType type) {
  switch (type) {
  case COLUMN_TYPE_INT:
    return "INT";
  case COLUMN_TYPE_TEXT:
    return "TEXT";
  default:
    return "UNKNOWN";
  }
}

SemanticResult semantic_validate_data_types(Column *columns, int num_columns,
                                            Value *values, char **expected_type,
                                            char **found_type,
                                            char **column_name) {
  for (int i = 0; i < num_columns; i++) {
    if (columns[i].type != values[i].type) {
      *expected_type = data_type_to_string(columns[i].type);
      *found_type = data_type_to_string(values[i].type);
      *column_name = columns[i].name;
      return SEMANTIC_TYPE_MISMATCH;
    }
  }
  return SEMANTIC_SUCCESS;
}
