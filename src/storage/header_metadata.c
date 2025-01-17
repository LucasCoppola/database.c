#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/hashmap.h"
#include "../../include/storage.h"

void header_tables_store(Database *db) {
  uint32_t num_tables = db->pager->num_tables;
  printf("Storing %d tables\n", num_tables);

  lseek(db->pager->file_descriptor, 0, SEEK_SET);
  ftruncate(db->pager->file_descriptor, 0);
  write(db->pager->file_descriptor, &num_tables, sizeof(uint32_t));

  TableHeader header = {0};
  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (iterator == NULL) {
    printf("Failed to initialize hashmap iterator\n");
    return;
  }

  size_t header_index = 0;
  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);

    if (bucket != NULL) {
      // Fills the header struct with zeros
      memset(&header, 0, HEADER_SIZE);

      Table *table = bucket->value;
      strncpy(header.table_name, table->name, MAX_NAME_LENGTH);
      header.num_rows = table->num_rows;
      header.next_id = table->next_id;

      printf("Storing table %zu: %s with %d rows\n", header_index,
             header.table_name, header.num_rows);

      write(db->pager->file_descriptor, &header, HEADER_SIZE);
      header_index++;
      uint32_t header_offset = sizeof(uint32_t) + (HEADER_SIZE * num_tables);
      if (header_offset > db->pager->file_length) {
        db->pager->file_length = header_offset;
      }
    }
  }

  pager_flush_all(db->pager);
  fsync(db->pager->file_descriptor);
  hashmap_iterator_free(iterator);
}

void header_tables_restore(Pager *pager, HashMap *map) {
  lseek(pager->file_descriptor, 0, SEEK_SET);

  uint32_t num_tables;
  ssize_t bytes_read =
      read(pager->file_descriptor, &num_tables, sizeof(uint32_t));
  if (bytes_read == 0) {
    // New database file, initialize with 0 tables
    pager->num_tables = 0;
    return;
  } else if (bytes_read != sizeof(uint32_t)) {
    printf("Error reading number of tables\n");
    return;
  }
  pager->num_tables = num_tables;
  printf("Found %d tables to restore\n", num_tables);

  TableHeader header = {0};
  size_t tables_restored = 0;

  // New variable to track the current header position
  off_t header_pos = sizeof(uint32_t); // Start after num_tables

  while (tables_restored < num_tables) {
    lseek(pager->file_descriptor, header_pos, SEEK_SET);
    bytes_read = read(pager->file_descriptor, &header, HEADER_SIZE);
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

    header_pos += HEADER_SIZE;

    if (header.table_name[0] != '\0') {
      printf("Restoring table %zu: %s, num_rows: %d, next_id: %d\n",
             tables_restored, header.table_name, header.num_rows,
             header.next_id);

      Table *table = malloc(sizeof(Table));
      if (table == NULL) {
        printf("Memory allocation failed for table\n");
        continue;
      }

      table->pager = pager;
      table->num_rows = header.num_rows;
      table->next_id = header.next_id;
      strncpy(table->name, header.table_name, MAX_NAME_LENGTH);

      hashmap_set(map, table->name, table);
      header_pages_restore(header.num_rows, pager);
      tables_restored++;
    }
  }
}

void header_pages_restore(uint32_t num_rows, Pager *pager) {
  for (uint32_t i = 0; i < num_rows; i++) {
    pager_load_page(pager, i);
  }
}
