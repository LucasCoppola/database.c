#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/logger.h"

DatabaseResult create_database(Database **out_db) {
  Database *db = malloc(sizeof(Database));
  if (db == NULL) {
    return DATABASE_ALLOC_ERROR;
  }

  HashMap *map = NULL;
  HashMapResult map_result = hashmap_initialize(10, &map);
  if (map_result != HASHMAP_SUCCESS) {
    LOG_ERROR("hashmap", map_result);
    free(db);
    return DATABASE_HASHMAP_INIT_ERROR;
  }

  db->tables = map;
  *out_db = db;
  return DATABASE_SUCCESS;
}

void free_database(Database *db) {
  if (!db)
    return;

  hashmap_free(db->tables);
  free(db);
}
