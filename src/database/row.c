#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/storage.h"

RowResult insert_row(Table *table, Row row) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  if (strlen(row.username) >= MAX_NAME_LENGTH ||
      strlen(row.email) >= MAX_NAME_LENGTH) {
    return ROW_NAME_TOO_LONG;
  }

  void *slot = get_row_slot(table, table->num_rows);
  row.id = table->next_id++;
  serialize_row(&row, slot);
  table->num_rows++;

  uint32_t page_num = table->num_rows / ROWS_PER_PAGE;
  pager_flush(table->pager, page_num);

  return ROW_SUCCESS;
}

RowResult select_rows(Table *table) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  Row row;
  for (uint32_t i = 0; i < table->num_rows; i++) {
    deserialize_row(get_row_slot(table, i), &row);
    printf("(%d, %s, %s)\n", row.id, row.username, row.email);
  }
  return ROW_SUCCESS;
}

RowResult delete_row(Table *table, uint32_t row_id) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  int found = 0;

  for (uint32_t i = 0; i < table->num_rows; i++) {
    Row row;
    deserialize_row(get_row_slot(table, i), &row);

    if (row.id == row_id) {
      found = 1;
    }

    if (found && i < table->num_rows - 1) {
      void *current_slot = get_row_slot(table, i);
      void *next_slot = get_row_slot(table, i + 1);
      memcpy(current_slot, next_slot, sizeof(Row));
    }
  }

  if (found) {
    table->num_rows--;
    return ROW_SUCCESS;
  }

  return ROW_NOT_FOUND;
}
