#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/hashmap.h"
#include "../../include/logger.h"
#include "../../include/storage.h"

void header_tables_restore(Pager *pager, HashMap *map) {
  lseek(pager->file_descriptor, 0, SEEK_SET);
  pager->num_tables = header_tables_count_read(pager->file_descriptor);
  if (pager->num_tables == (uint32_t)-1) {
    return;
  }

  off_t header_pos = sizeof(uint32_t);
  for (size_t i = 0; i < pager->num_tables; i++) {
    TableHeader header;
    if (!header_table_read(pager, &header_pos, &header)) {
      break;
    }
    header_table_initialize(&header, pager, map);
  }
}

bool header_table_read(Pager *pager, off_t *header_pos, TableHeader *header) {
  lseek(pager->file_descriptor, *header_pos, SEEK_SET);
  ssize_t bytes_read = read(pager->file_descriptor, header, HEADER_SIZE);
  *header_pos += HEADER_SIZE;
  return bytes_read == HEADER_SIZE;
}

void header_table_initialize(TableHeader *header, Pager *pager, HashMap *map) {
  Table *table = malloc(sizeof(Table));
  if (!table)
    return;

  table->pager = pager;
  table->num_rows = header->num_rows;
  table->next_id = header->next_id;
  table->page_offset = header->page_offset;
  strncpy(table->name, header->table_name, MAX_NAME_LENGTH);

  HashMapResult result = hashmap_set(map, table->name, table);
  if (result != HASHMAP_SUCCESS) {
    LOG_ERROR("hashmap", result);
    return;
  }

  for (uint32_t i = 0; i < table->num_rows; i++) {
    pager_load_page(pager, i, table);
  }
}

uint32_t header_tables_count_read(int file_descriptor) {
  uint32_t num_tables;
  ssize_t bytes_read = read(file_descriptor, &num_tables, sizeof(uint32_t));

  if (bytes_read == 0) {
    return 0; // New database file, 0 tables
  } else if (bytes_read != sizeof(uint32_t)) {
    printf("Error reading number of tables\n");
    return -1;
  }

  return num_tables;
}
