#include <errno.h>
#include <fcntl.h>
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

  ftruncate(db->pager->file_descriptor, 0);
  lseek(db->pager->file_descriptor, 0, SEEK_SET);

  size_t header_index = 0;
  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);

    if (bucket != NULL) {
      memset(&header, 0, sizeof(TableHeader)); // clears the header

      Table *table = bucket->value;
      strncpy(header.table_name, table->name, MAX_NAME_LENGTH);
      header.num_rows = table->num_rows;
      header.next_id = table->next_id;

      off_t position = header_index * sizeof(TableHeader);
      lseek(db->pager->file_descriptor, position, SEEK_SET);
      write(db->pager->file_descriptor, &header, sizeof(TableHeader));

      header_index++;
    }
  }
  hashmap_iterator_free(iterator);
}

void header_tables_restore(Pager *pager, HashMap *map) {
  TableHeader header = {0};
  lseek(pager->file_descriptor, 0, SEEK_SET);

  // Read headers until we reach EOF
  while (1) {
    ssize_t bytes_read =
        read(pager->file_descriptor, &header, sizeof(TableHeader));

    if (bytes_read == 0) { // EOF
      break;
    } else if (bytes_read == -1) {
      printf("Error reading headers from file: %s\n", strerror(errno));
      return;
    } else if (bytes_read != sizeof(TableHeader)) {
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
    }
  }
}
