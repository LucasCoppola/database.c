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

RowResult insert_row(Table *out_table, ASTNode *node) {
  if (out_table == NULL) {
    return ROW_INVALID_TABLE;
  }

  Row *row = NULL;
  RowResult init_result = initialize_row(out_table, &row);
  if (init_result != ROW_SUCCESS) {
    DEBUG_LOG("row", "initialize", init_result);
    return init_result;
  }

  for (uint32_t i = 0; i < row->num_columns; i++) {
    row->values[i].type = node->insert_rows.values[i].type;

    if (node->insert_rows.values[i].type == COLUMN_TYPE_TEXT) {
      row->values[i].string_value =
          strdup(node->insert_rows.values[i].string_value);
    } else {
      row->values[i].int_value = node->insert_rows.values[i].int_value;
    }
  }

  Cursor *cursor = table_end(out_table);
  uint32_t rows_per_page = calculate_rows_per_page(out_table);
  uint32_t page_num = out_table->num_rows / rows_per_page;
  uint32_t row_offset = out_table->num_rows % rows_per_page;

  if (row_offset == 0) {
    PagerResult result = pager_page_alloc(page_num, out_table);
    if (result != PAGER_SUCCESS) {
      free(cursor);
      free_row(row);
      DEBUG_LOG("pager", "allocation", result);
      return ROW_ALLOC_PAGE_ERROR;
    }
  }

  void *page = NULL;
  PagerResult result =
      pager_page_load(out_table->pager, page_num, out_table, &page);
  if (result != PAGER_SUCCESS) {
    free(cursor);
    free_row(row);
    DEBUG_LOG("pager", "load", result);
    return ROW_GET_PAGE_ERROR;
  }

  void *row_location = cursor_value(cursor);
  serialize_row(row, out_table, row_location);
  out_table->num_rows++;

  PagerResult pager_result =
      pager_page_flush(out_table->pager, page_num, out_table);
  if (pager_result != PAGER_SUCCESS) {
    DEBUG_LOG("pager", "flush", pager_result);
    free_row(row);
    free(cursor);
    return ROW_FLUSH_PAGE_ERROR;
  }

  free_row(row);
  free(cursor);
  return ROW_SUCCESS;
}

RowResult select_row(Table *table, ASTNode *node) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  Cursor *cursor = table_start(table);

  Row row;
  row.values = malloc(table->num_columns * sizeof(Value));
  if (!row.values) {
    free(cursor);
    return ROW_VALUES_ALLOC_ERROR;
  }

  while (!cursor->end_of_table) {
    deserialize_row(cursor_value(cursor), &row, table);
    print_row(row, node, table);
    cursor_advance(cursor);
  }

  free(row.values);
  free(cursor);
  return ROW_SUCCESS;
}

RowResult delete_row(Table *table, uint32_t row_id) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  Cursor *cursor = table_start(table);
  Row row;
  int found = 0;

  while (!cursor->end_of_table) {
    deserialize_row(cursor_value(cursor), &row, table);
    if (row.id == row_id) {
      found = 1;
    }

    // shift row to fill the gap
    if (found && !cursor->end_of_table) {
      void *current_slot = cursor_value(cursor);
      void *next_slot = cursor_value(cursor) + sizeof(Row);
      memcpy(current_slot, next_slot, sizeof(Row));
    }

    cursor_advance(cursor);
  }

  if (found) {
    table->num_rows--;
    return ROW_SUCCESS;
  }

  free(cursor);
  return ROW_NOT_FOUND;
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
