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
  TableHeader header = {0};
  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (iterator == NULL) {
    printf("Failed to initialize hashmap iterator\n");
    return;
  }

  lseek(db->pager->file_descriptor, 0, SEEK_SET);
  ftruncate(db->pager->file_descriptor, 0);

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

      write(db->pager->file_descriptor, &header, HEADER_SIZE);
      header_index++;
    }
  }

  pager_flush_all(db->pager);
  fsync(db->pager->file_descriptor);
  hashmap_iterator_free(iterator);
}

void header_tables_restore(Pager *pager, HashMap *map) {
  TableHeader header = {0};
  lseek(pager->file_descriptor, 0, SEEK_SET);

  // Read headers until we reach EOF
  while (1) {
    ssize_t bytes_read = read(pager->file_descriptor, &header, HEADER_SIZE);
    if (bytes_read == 0) { // EOF
      break;
    } else if (bytes_read == -1) {
      printf("Error reading headers from file: %s\n", strerror(errno));
      return;
    } else if (bytes_read != HEADER_SIZE) {
      printf("Incomplete header read\n");
      break;
    }

    if (header.table_name[0] != '\0') {
      printf("Restoring table: %s, num_rows: %d, next_id: %d\n",
             header.table_name, header.num_rows, header.next_id);

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
    }
  }
}

void header_pages_restore(uint32_t num_rows, Pager *pager) {
  uint32_t num_pages = (num_rows + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
  for (uint32_t i = 0; i < num_pages; i++) {
    pager_load_page(pager, i);
  }
}
