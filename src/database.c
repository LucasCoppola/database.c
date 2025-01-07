#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"

Database *create_database() {
  Database *db = malloc(sizeof(Database));
  if (db == NULL) {
    printf("Failed to allocate db\n");
    return NULL;
  }

  db->tables_head = NULL;
  return db;
}

void free_database(Database *db) {
  if (db) {
    free_table(db->tables_head);
    free(db);
  }
}
