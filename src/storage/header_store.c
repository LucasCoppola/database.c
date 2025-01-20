#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/hashmap.h"
#include "../../include/storage.h"

// Initializes and returns the header struct for a table
TableHeader initialize_table_header(Table *table) {
  TableHeader header = {0};
  strncpy(header.table_name, table->name, MAX_NAME_LENGTH);
  header.num_rows = table->num_rows;
  header.next_id = table->next_id;
  return header;
}

// Writes the number of tables to the file
void write_num_tables(int file_descriptor, uint32_t num_tables) {
  lseek(file_descriptor, 0, SEEK_SET);
  ftruncate(file_descriptor, 0);
  write(file_descriptor, &num_tables, sizeof(uint32_t));
}

// Writes a single table header to the file
void write_table_header(int file_descriptor, TableHeader *header,
                        size_t header_index) {
  printf("Storing table %zu: %s with %d rows\n", header_index,
         header->table_name, header->num_rows);
  write(file_descriptor, header, HEADER_SIZE);
}

// Main function to store headers and flush changes
void header_tables_store(Database *db) {
  printf("Storing %d tables\n", db->pager->num_tables);
  write_num_tables(db->pager->file_descriptor, db->pager->num_tables);

  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (iterator == NULL) {
    printf("Failed to initialize hashmap iterator\n");
    return;
  }

  size_t header_index = 0;
  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);
    if (bucket != NULL) {
      Table *table = bucket->value;
      TableHeader header = initialize_table_header(table);
      write_table_header(db->pager->file_descriptor, &header, header_index);
      header_index++;
    }
  }

  pager_flush_all(db->pager);
  fsync(db->pager->file_descriptor);
  hashmap_iterator_free(iterator);
}
