#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/hashmap.h"
#include "../../include/storage.h"

// Reads the number of tables from the file
uint32_t read_num_tables(int file_descriptor) {
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

// Reads a single table header from the file
ssize_t read_table_header(int file_descriptor, off_t *header_pos,
                          TableHeader *header) {
  lseek(file_descriptor, *header_pos, SEEK_SET);
  ssize_t bytes_read = read(file_descriptor, header, HEADER_SIZE);
  *header_pos += HEADER_SIZE;
  return bytes_read;
}

// Initializes a table from the header and adds it to the hashmap
void initialize_table_from_header(TableHeader *header, Pager *pager,
                                  HashMap *map) {
  Table *table = malloc(sizeof(Table));
  if (table == NULL) {
    printf("Memory allocation failed for table\n");
    return;
  }

  table->pager = pager;
  table->num_rows = header->num_rows;
  table->next_id = header->next_id;
  strncpy(table->name, header->table_name, MAX_NAME_LENGTH);

  hashmap_set(map, table->name, table);
  printf("Restoring table: %s, num_rows: %d, next_id: %d\n", table->name,
         table->num_rows, table->next_id);

  for (uint32_t i = 0; i < table->num_rows; i++) {
    pager_load_page(pager, i, table);
  }
}

// Main function to restore headers
void header_tables_restore(Pager *pager, HashMap *map) {
  lseek(pager->file_descriptor, 0, SEEK_SET);
  uint32_t num_tables = read_num_tables(pager->file_descriptor);
  if (num_tables == (uint32_t)-1) {
    return;
  }

  pager->num_tables = num_tables;
  printf("Found %d tables to restore\n", num_tables);

  TableHeader header = {0};
  size_t tables_restored = 0;
  off_t header_pos = sizeof(uint32_t); // Start after num_tables

  while (tables_restored < num_tables) {
    ssize_t bytes_read =
        read_table_header(pager->file_descriptor, &header_pos, &header);

    if (bytes_read == 0) { // EOF
      printf("Unexpected EOF after restoring %zu tables\n", tables_restored);
      break;
    } else if (bytes_read == -1) {
      printf("Error reading headers from file: %s\n", strerror(errno));
      return;
    } else if (bytes_read != HEADER_SIZE) {
      printf("Incomplete header read: %zd bytes\n", bytes_read);
      break;
    }

    if (header.table_name[0] != '\0') {
      initialize_table_from_header(&header, pager, map);
      tables_restored++;
    }
  }
}
