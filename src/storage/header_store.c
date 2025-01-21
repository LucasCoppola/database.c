#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/hashmap.h"
#include "../../include/storage.h"

// Writes the number of tables to the file
void write_num_tables(int file_descriptor, uint32_t num_tables) {
  lseek(file_descriptor, 0, SEEK_SET);
  // ftruncate(file_descriptor, 0);
  write(file_descriptor, &num_tables, sizeof(uint32_t));
}

// Writes a single table header to the file
void write_table_header(int file_descriptor, Table *table, size_t header_index,
                        off_t page_data_offset) {
  TableHeader header = {0};
  strncpy(header.table_name, table->name, MAX_NAME_LENGTH);
  header.num_rows = table->num_rows;
  header.next_id = table->next_id;
  header.page_offset = page_data_offset;
  // Calculate where this header should be written
  off_t header_position = sizeof(uint32_t) + (header_index * HEADER_SIZE);

  printf("Writing table header at position: %ld\n", header_position);
  printf("Table %s: num_rows=%d, next_id=%d, page_offset=%u\n",
         header.table_name, header.num_rows, header.next_id,
         header.page_offset);

  lseek(file_descriptor, header_position, SEEK_SET);
  write(file_descriptor, &header, HEADER_SIZE);
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
  off_t header_section_size =
      sizeof(uint32_t) + (HEADER_SIZE * db->pager->num_tables);

  printf("Initial page data offset: %ld\n", header_section_size);

  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);
    if (bucket != NULL) {
      Table *table = bucket->value;

      printf("Processing table %zu: %s with %d rows at %d\n", header_index,
             table->name, table->num_rows, table->page_offset);

      write_table_header(db->pager->file_descriptor, table, header_index,
                         table->page_offset);

      pager_flush_all(db->pager, table);
      header_index++;

      // Calculate space needed for this table's data
      uint32_t num_pages =
          (table->num_rows + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
      if (num_pages == 0) {
        num_pages = 1;
      }
    }
  }

  hashmap_iterator_free(iterator);
  fsync(db->pager->file_descriptor);
}
