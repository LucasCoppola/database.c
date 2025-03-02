#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"

#include "core/row.h"
#include "parser/ast.h"
#include "storage/pager.h"

uint32_t calculate_row_size(Table *table) {
  uint32_t row_size = sizeof(uint32_t); // row.id
  row_size += sizeof(uint32_t);         // row.row_size
  for (uint32_t i = 0; i < table->num_columns; i++) {
    row_size += sizeof(DataType);
    switch (table->columns[i].type) {
    case COLUMN_TYPE_INT:
      row_size += sizeof(uint32_t);
      break;
    case COLUMN_TYPE_TEXT:
      row_size += sizeof(uint32_t);
      row_size += MAX_NAME_LENGTH;
      break;
    default:
      break;
    }
  }
  return row_size;
}

uint32_t calculate_rows_per_page(Table *table) {
  uint32_t row_size = calculate_row_size(table);
  return PAGE_SIZE / row_size;
}

uint32_t calculate_max_rows(Table *table) {
  return calculate_rows_per_page(table) * TABLE_MAX_PAGES;
}

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

void print_value(Value value, DataType type) {
  switch (type) {
  case COLUMN_TYPE_INT:
    printf("%d", value.int_value);
    break;
  case COLUMN_TYPE_TEXT:
    printf("%s", value.string_value);
    break;
  default:
    printf("Unknown type");
  }
}

void print_row(Row row, ASTNode *node, Table *table) {
  printf("(");

  if (node->select_rows.select_all) {
    for (uint32_t i = 0; i < row.num_columns; i++) {
      print_value(row.values[i], table->columns[i].type);
      if (i < row.num_columns - 1) {
        printf(", ");
      }
    }
  } else {
    for (int i = 0; i < node->select_rows.num_columns; i++) {
      int col_index = -1;
      for (uint32_t j = 0; j < table->num_columns; j++) {
        if (strcmp(node->select_rows.select_columns[i],
                   table->columns[j].name) == 0) {
          col_index = j;
          break;
        }
      }

      if (col_index == -1) {
        printf("No such column: '%s'", node->select_rows.select_columns[i]);
      } else {
        print_value(row.values[col_index], table->columns[col_index].type);
      }

      if (i < node->select_rows.num_columns - 1) {
        printf(", ");
      }
    }
  }

  printf(")\n");
}
