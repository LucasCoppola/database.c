#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"
#include "parser/keywords.h"

#include "core/row.h"

DataType map_column_type(char *type) {
  if (strcmp(type, KEYWORD_INT) == 0 || strcmp(type, KEYWORD_INTEGER) == 0) {
    return COLUMN_TYPE_INT;
  } else if (strcmp(type, KEYWORD_TEXT) == 0) {
    return COLUMN_TYPE_TEXT;
  } else if (strcmp(type, KEYWORD_REAL) == 0) {
    return COLUMN_TYPE_REAL;
  } else if (strcmp(type, KEYWORD_BOOL) == 0 ||
             strcmp(type, KEYWORD_BOOLEAN) == 0) {
    return COLUMN_TYPE_BOOL;
  } else {
    printf("Unknown column type %s\n", type);
    return COLUMN_TYPE_UNKNOWN;
  }
}

char *data_type_to_string(DataType type) {
  switch (type) {
  case COLUMN_TYPE_INT:
    return KEYWORD_INT;
  case COLUMN_TYPE_REAL:
    return KEYWORD_REAL;
  case COLUMN_TYPE_TEXT:
    return KEYWORD_TEXT;
  case COLUMN_TYPE_BOOL:
    return KEYWORD_BOOLEAN;
  default:
    return "UNKNOWN";
  }
}

Value *convert_value(const char *value_str, DataType type) {
  Value *value = malloc(sizeof(Value));
  if (!value) {
    fprintf(stderr, "Failed to allocate memory for value");
    return NULL;
  }
  value->type = type;

  switch (type) {
  case COLUMN_TYPE_INT:
    value->int_value = atoi(value_str);
    break;
  case COLUMN_TYPE_REAL:
    value->real_value = strtod(value_str, NULL);
    break;
  case COLUMN_TYPE_TEXT:
    value->string_value = strdup(value_str);
    break;
  case COLUMN_TYPE_BOOL:
    value->bool_value = (strcasecmp(value_str, "true") == 0);
    break;
  default:
    fprintf(stderr, "Unknown type in convert_value\n");
  }
  return value;
}

void print_value(Value value, DataType type) {
  switch (type) {
  case COLUMN_TYPE_INT:
    printf("%d", value.int_value);
    break;
  case COLUMN_TYPE_TEXT:
    printf("%s", value.string_value);
    break;
  case COLUMN_TYPE_REAL:
    printf("%g", value.real_value);
    break;
  case COLUMN_TYPE_BOOL:
    printf("%s", value.bool_value ? "true" : "false");
    break;
  default:
    printf("Unknown type");
  }
}
