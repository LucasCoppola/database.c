#include "parser/semantic_analyzer.h"
#include "utils/convertions.h"

SemanticResult semantic_validate_data_types(Column *columns, int num_columns,
                                            Value **values,
                                            char **out_expected_type,
                                            char **out_found_type,
                                            char **out_column_name) {
  for (int i = 0; i < num_columns; i++) {
    if (columns[i].type != values[i]->type) {
      *out_expected_type = data_type_to_string(columns[i].type);
      *out_found_type = data_type_to_string(values[i]->type);
      *out_column_name = columns[i].name;
      return SEMANTIC_TYPE_MISMATCH;
    }
  }
  return SEMANTIC_SUCCESS;
}

bool semantic_is_operator_compatible(DataType col_type, char op) {
  if (op != OP_LESS && op != OP_GREATER && op != OP_EQUAL) {
    return false;
  }

  switch (col_type) {
  case COLUMN_TYPE_TEXT:
    return (op == OP_EQUAL);
  case COLUMN_TYPE_BOOL:
    return (op == OP_EQUAL);
  case COLUMN_TYPE_INT:
  case COLUMN_TYPE_REAL:
    return true;

  default:
    return false;
  }
}
