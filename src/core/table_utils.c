#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "core/database.h"
#include "core/row.h"
#include "core/table.h"

#include "storage/cursor.h"
#include "storage/pager.h"
#include "utils/hashmap.h"

TableResult table_initialize(Table *table, char *name, Database *db) {
  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';
  table->next_id = 1;
  table->num_rows = 0;
  table->pager = db->pager;
  table->page_offset = table_position(db);
  memset(table->pages, 0, sizeof(void *) * TABLE_MAX_PAGES);

  table->pager->num_tables++;
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
  uint32_t num_pages = (table->num_rows + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
  return num_pages == 0 ? 1 : num_pages;
}
