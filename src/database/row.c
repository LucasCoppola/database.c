#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/storage.h"

void insert_row(Table *table, Row row) {
  if (strlen(row.username) >= MAX_NAME_LENGTH ||
      strlen(row.email) >= MAX_NAME_LENGTH) {
    printf("name is too long (max %d characters)\n", MAX_NAME_LENGTH - 1);
    return;
  }

  void *slot = get_row_slot(table, table->num_rows);
  row.id = table->next_id++;
  serialize_row(&row, slot);
  table->num_rows++;
};

void select_rows(Table *table) {
  Row row;
  for (uint32_t i = 0; i < table->num_rows; i++) {
    deserialize_row(get_row_slot(table, i), &row);
    printf("(%d, %s, %s)\n", row.id, row.username, row.email);
  }
}

void delete_row(Table *table, uint32_t row_id) {
  int found = 0;

  for (uint32_t i = 0; i < table->num_rows; i++) {
    Row row;
    deserialize_row(get_row_slot(table, i), &row);

    if (row.id == row_id) {
      found = 1;
    }

    if (found && i < table->num_rows - 1) {
      // Shift the next row up to the current position
      void *current_slot = get_row_slot(table, i);
      void *next_slot = get_row_slot(table, i + 1);
      memcpy(current_slot, next_slot, sizeof(Row));
    }
  }

  if (found) {
    table->num_rows--;
  } else {
    printf("Row with id %d not found.\n", row_id);
  }
}
