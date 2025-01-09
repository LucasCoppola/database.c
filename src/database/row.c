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
