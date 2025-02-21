#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "core/row.h"
#include "core/table.h"

#include "core/hashmap.h"
#include "storage/cursor.h"
#include "storage/pager.h"
#include "utils/logger.h"

RowResult insert_row(Table *table, Row *row) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  Cursor *cursor = table_end(table);
  uint32_t page_num = table->num_rows / ROWS_PER_PAGE;
  uint32_t row_offset = table->num_rows % ROWS_PER_PAGE;
  void *page = NULL;

  printf("Inserting row into table %s at page %u, row offset %u\n", table->name,
         page_num, row_offset);

  if (row_offset == 0) {
    PagerResult result = pager_page_alloc(page_num, table);
    if (result != PAGER_SUCCESS) {
      LOG_ERROR("pager", "allocation", result);
      return ROW_ALLOC_PAGE_ERROR;
    }
  }

  PagerResult result = pager_page_load(table->pager, page_num, table, &page);
  if (result != PAGER_SUCCESS) {
    free(cursor);
    LOG_ERROR("pager", "load", result);
    return ROW_GET_PAGE_ERROR;
  }

  row->id = table->next_id++;
  void *row_location = cursor_value(cursor);
  printf("Writing row at offset %ld within page\n",
         (char *)row_location - (char *)page);

  serialize_row(row, row_location);
  table->num_rows++;

  PagerResult pager_result = pager_page_flush(table->pager, page_num, table);
  if (pager_result != PAGER_SUCCESS) {
    LOG_ERROR("pager", "flush", pager_result);
    return ROW_FLUSH_PAGE_ERROR;
  }

  free(cursor);
  return ROW_SUCCESS;
}

RowResult select_rows(Table *table) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }
  Cursor *cursor = table_start(table);

  Row row;
  while (!cursor->end_of_table) {
    deserialize_row(cursor_value(cursor), &row);
    printf("(%d, %s)\n", row.id, row.name);
    cursor_advance(cursor);
  }

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
    deserialize_row(cursor_value(cursor), &row);
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
