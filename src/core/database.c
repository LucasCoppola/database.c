#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "core/database.h"
#include "core/table.h"

#include "storage/pager.h"
#include "storage/table_header.h"
#include "utils/hashmap.h"
#include "utils/logger.h"

DatabaseResult database_open(Database **out_db, const char *filename) {
  Database *db = malloc(sizeof(Database));
  if (db == NULL) {
    return DATABASE_ALLOC_ERROR;
  }

  Pager *pager = NULL;
  PagerResult pager_result = pager_init(filename, &pager);
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

  header_tables_store(db);

  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);
    if (bucket && bucket->value) {
      Table *table = bucket->value;
      table_free(table);
    }
  }

  hashmap_iterator_free(iterator);
  pager_close(db->pager);
  database_free(db);
}

void database_free(Database *db) {
  if (!db)
    return;

  hashmap_free(db->tables);
  free(db);
}
