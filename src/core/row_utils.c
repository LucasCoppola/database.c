#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"

#include "core/row.h"

Value convert_value(const char *value_str, DataType type) {
  Value value;
  value.type = type;

  switch (type) {
  case COLUMN_TYPE_INT:
    value.int_value = atoi(value_str);
    break;
  case COLUMN_TYPE_TEXT:
    value.string_value = strdup(value_str);
    break;
  case COLUMN_TYPE_UNKNOWN:
    fprintf(stderr, "Unknown column type\n");
    break;
  }

  return value;
}
