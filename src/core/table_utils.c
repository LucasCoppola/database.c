#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "core/database.h"
#include "core/table.h"

#include "core/hashmap.h"
#include "core/row.h"
#include "storage/pager.h"

TableResult table_initialize(Table *table, char *name, Database *db) {
  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';
  table->next_id = 1;
  table->num_rows = 0;

  table->pager = db->pager;
  table->pager->num_tables++;
  table->page_offset = table_position(db);

  table->num_columns = 0;
  table->columns = NULL;

  memset(table->pages, 0, sizeof(void *) * TABLE_MAX_PAGES);
  return TABLE_SUCCESS;
}

TableResult table_columns_set(Table *out_table, Column *columns,
                              uint32_t num_cols) {
  out_table->columns = malloc(sizeof(Column) * num_cols);
  if (out_table->columns == NULL) {
    return TABLE_ALLOC_ERROR;
  }

  for (uint32_t i = 0; i < num_cols; i++) {
    out_table->columns[i].name = strdup(columns[i].name);
    if (!out_table->columns[i].name) {
      return TABLE_ALLOC_ERROR;
    }
    out_table->columns[i].type = columns[i].type;
  }

  out_table->num_columns = num_cols;
  return TABLE_SUCCESS;
}

off_t table_position(Database *db) {
  off_t position = sizeof(uint32_t) + (MAX_TABLES * HEADER_SIZE);

  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (!iterator)
    return position;

  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);
    if (bucket) {
      Table *table = bucket->value;
      position += table_pages_count(table) * PAGE_SIZE;
    }
  }
  hashmap_iterator_free(iterator);
  return position;
}

uint32_t table_pages_count(Table *table) {
  uint32_t rows_per_page = calculate_rows_per_page(table);
  uint32_t num_pages = (table->num_rows + rows_per_page - 1) / rows_per_page;
  return num_pages == 0 ? 1 : num_pages;
}
