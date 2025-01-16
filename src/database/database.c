#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/logger.h"
#include "../include/storage.h"

DatabaseResult database_open(Database **out_db, const char *filename) {
  Database *db = malloc(sizeof(Database));
  if (db == NULL) {
    return DATABASE_ALLOC_ERROR;
  }

  Pager *pager = NULL;
  PagerResult pager_result = pager_open(filename, &pager);
  if (pager_result != PAGER_SUCCESS) {
    LOG_ERROR("pager", pager_result);
    free(db);
    return DATABASE_PAGER_INIT_ERROR;
  }

  HashMap *map = NULL;
  HashMapResult map_result = hashmap_initialize(10, &map);
  if (map_result != HASHMAP_SUCCESS) {
    LOG_ERROR("hashmap", map_result);
    free(pager);
    free(db);
    return DATABASE_HASHMAP_INIT_ERROR;
  }

  header_tables_restore(pager, map);

  db->pager = pager;
  db->tables = map;
  *out_db = db;
  return DATABASE_SUCCESS;
}

void database_close(Database *db) {
  if (!db)
    return;

  pager_flush_all(db->pager);
  header_tables_store(db);
  pager_close(db->pager);
  database_free(db);
}

void database_free(Database *db) {
  if (!db)
    return;

  hashmap_free(db->tables);
  free(db);
}
