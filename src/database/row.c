#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/storage.h"

RowResult insert_row(Table *table, Row *row) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  if (strlen(row->username) >= MAX_NAME_LENGTH ||
      strlen(row->email) >= MAX_NAME_LENGTH) {
    return ROW_NAME_TOO_LONG;
  }

  Cursor *cursor = table_end(table);
  row->id = table->next_id++;
  serialize_row(row, cursor_value(cursor));
  table->num_rows++;

  uint32_t page_num = table->num_rows / ROWS_PER_PAGE;
  pager_flush(table->pager, page_num);

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
    printf("(%d, %s, %s)\n", row.id, row.username, row.email);
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
