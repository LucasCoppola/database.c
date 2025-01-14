#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/storage.h"

void header_tables_restore(Pager *pager, HashMap *map) {
  TableHeader headers[MAX_TABLES];
  lseek(pager->file_descriptor, 0, SEEK_SET);
  read(pager->file_descriptor, headers, HEADER_SIZE);

  // Restore tables from headers
  for (int i = 0; i < MAX_TABLES; i++) {
    if (headers[i].table_name[0] != '\0') { // If table exists
      Table *table = malloc(sizeof(Table));
      table->pager = pager;
      table->num_rows = headers[i].num_rows;
      table->next_id = headers[i].next_id;
      strncpy(table->name, headers[i].table_name, MAX_NAME_LENGTH);
      hashmap_set(map, table->name, table);
    }
  }
}

void header_tables_store(Database *db) {
  // Save header for users table
  TableHeader header = {0}; // Initialize to 0
  Table *users_table = NULL;

  if (find_table(db, "users", &users_table) == TABLE_SUCCESS) {
    strncpy(header.table_name, "users", MAX_NAME_LENGTH);
    header.num_rows = users_table->num_rows;
    header.next_id = users_table->next_id;

    // Write header to start of file
    lseek(db->pager->file_descriptor, 0, SEEK_SET);
    write(db->pager->file_descriptor, &header, sizeof(TableHeader));

    // Close table and cleanup
    close_table(users_table, db->pager);
  }
}
