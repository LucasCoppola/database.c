#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "core/database.h"
#include "core/table.h"

#include "storage/pager.h"
#include "storage/table_header.h"

#include "utils/hashmap.h"
#include "utils/logger.h"

void header_tables_store(Database *db) {
  header_tables_count_write(db->pager, db->pager->num_tables);
  header_tables_write(db);
  fsync(db->pager->file_descriptor);
}

void header_tables_write(Database *db) {
  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (!iterator)
    return;

  for (size_t i = 0; hashmap_iterator_has_next(iterator); i++) {
    Table *table = hashmap_iterator_next(iterator)->value;
    header_table_write(db->pager, table, i);
    PagerResult result = pager_pages_flush(db->pager, table);
    if (result != PAGER_SUCCESS) {
      LOG_ERROR("pager", result);
      return;
    }
  }

  hashmap_iterator_free(iterator);
}

void header_table_write(Pager *pager, Table *table, size_t idx) {
  TableHeader header = {0};
  strncpy(header.table_name, table->name, MAX_NAME_LENGTH);
  header.num_rows = table->num_rows;
  header.next_id = table->next_id;
  header.page_offset = table->page_offset;
  header.num_columns = table->num_columns;

  for (uint32_t i = 0; i < table->num_columns; i++) {
    strncpy(header.columns[i].name, table->columns[i].name, MAX_NAME_LENGTH);
    header.columns[i].type = table->columns[i].type;
  }

  printf("table name: %s\n", header.table_name);
  printf("num rows: %d\n", header.num_rows);
  printf("next id: %d\n", header.next_id);
  printf("page offset: %d\n", header.page_offset);
  printf("num columns: %d\n", header.num_columns);

  for (uint32_t i = 0; i < header.num_columns; i++) {
    printf("column name: %s\n", header.columns[i].name);
    printf("column type: %d\n", header.columns[i].type);
  }

  off_t header_position = sizeof(uint32_t) + (idx * HEADER_SIZE);
  lseek(pager->file_descriptor, header_position, SEEK_SET);
  write(pager->file_descriptor, &header, HEADER_SIZE);
}

void header_tables_count_write(Pager *pager, uint32_t num_tables) {
  lseek(pager->file_descriptor, 0, SEEK_SET);
  write(pager->file_descriptor, &num_tables, sizeof(uint32_t));
}
