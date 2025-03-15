#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/table.h"

#include "core/row.h"
#include "parser/ast.h"
#include "parser/keywords.h"
#include "storage/pager.h"
#include "utils/convertions.h"

RowResult initialize_row(Table *table, Row **row) {
  *row = malloc(sizeof(Row));
  if (!*row) {
    fprintf(stderr, "Failed to allocate row.\n");
    return ROW_ALLOC_ERROR;
  }

  (*row)->id = table->next_id++;
  (*row)->num_columns = table->num_columns;
  (*row)->values = malloc((*row)->num_columns * sizeof(Value));
  if (!(*row)->values) {
    fprintf(stderr, "Failed to allocate row values.\n");
    free(*row);
    return ROW_VALUES_ALLOC_ERROR;
  }

  return ROW_SUCCESS;
}

void free_row(Row *row) {
  for (uint32_t i = 0; i < row->num_columns; i++) {
    if (row->values[i].type == COLUMN_TYPE_TEXT &&
        row->values[i].string_value) {
      free(row->values[i].string_value);
    }
  }
  free(row->values);
  free(row);
}

bool evaluate_where_condition(Row *row, WhereCondition *where, Table *table) {
  int col_index = -1;
  for (uint32_t i = 0; i < table->num_columns; i++) {
    if (strcmp(table->columns[i].name, where->column_name) == 0) {
      col_index = i;
      break;
    }
  }

  if (col_index < 0) {
    return false;
  }

  if (where->op == OP_EQUAL) {
    switch (row->values[col_index].type) {
    case COLUMN_TYPE_INT:
      return row->values[col_index].int_value == where->value->int_value;
    case COLUMN_TYPE_REAL:
      return row->values[col_index].real_value == where->value->real_value;
    case COLUMN_TYPE_TEXT:
      return strcmp(row->values[col_index].string_value,
                    where->value->string_value) == 0;
    case COLUMN_TYPE_BOOL:
      return row->values[col_index].bool_value == where->value->bool_value;
    default:
      return false;
    }
  } else if (where->op == OP_LESS) {
    switch (row->values[col_index].type) {
    case COLUMN_TYPE_INT:
      return row->values[col_index].int_value < where->value->int_value;
    case COLUMN_TYPE_REAL:
      return row->values[col_index].real_value < where->value->real_value;
    default:
      return false;
    }
  } else if (where->op == OP_GREATER) {
    switch (row->values[col_index].type) {
    case COLUMN_TYPE_INT:
      return row->values[col_index].int_value > where->value->int_value;
    case COLUMN_TYPE_REAL:
      return row->values[col_index].real_value > where->value->real_value;
    default:
      return false;
    }
  }

  return false;
}

uint32_t calculate_row_size(Table *table) {
  uint32_t row_size = sizeof(uint32_t); // row.id
  row_size += sizeof(uint32_t);         // row.row_size
  for (uint32_t i = 0; i < table->num_columns; i++) {
    row_size += sizeof(DataType);
    switch (table->columns[i].type) {
    case COLUMN_TYPE_INT:
      row_size += sizeof(uint32_t);
      break;
    case COLUMN_TYPE_REAL:
      row_size += sizeof(double);
      break;
    case COLUMN_TYPE_TEXT:
      row_size += sizeof(uint32_t);
      row_size += MAX_NAME_LENGTH;
      break;
    case COLUMN_TYPE_BOOL:
      row_size += sizeof(bool);
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
      print_value(row.values[col_index], table->columns[col_index].type);

      if (i < node->select_rows.num_columns - 1) {
        printf(", ");
      }
    }
  }

  printf(")\n");
}
