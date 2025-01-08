#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"

Database *create_database() {
  Database *db = malloc(sizeof(Database));
  if (db == NULL) {
    printf("Failed to allocate db\n");
    return NULL;
  }

  HashMap *map = NULL;
  HashMapResult map_result = hashmap_initialize(10, &map);
  if (map_result != HASHMAP_SUCCESS) {
    printf("Failed to initialize hashmap\n");
    free(db);
    return NULL;
  }

  return db;
}

void free_database(Database *db) {
  if (!db)
    return;

  hashmap_free(db->tables);
  free(db);
}
