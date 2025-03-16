#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "core/database.h"
#include "core/table.h"

#include "core/hashmap.h"
#include "storage/pager.h"
#include "storage/table_header.h"
#include "utils/convertions.h"
#include "utils/logger.h"

DatabaseResult database_open(Database **out_db, const char *filename) {
  Database *db = malloc(sizeof(Database));
  if (db == NULL) {
    return DATABASE_ALLOC_ERROR;
  }

  Pager *pager = NULL;
  PagerResult pager_result = pager_init(filename, &pager);
  if (pager_result != PAGER_SUCCESS) {
    DEBUG_LOG("pager", "init", pager_result);
    free(db);
    return DATABASE_PAGER_INIT_ERROR;
  }

  HashMap *map = NULL;
  HashMapResult map_result = hashmap_initialize(HASHMAP_CAPACITY, &map);
  if (map_result != HASHMAP_SUCCESS) {
    DEBUG_LOG("hashmap", "init", map_result);
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

void database_tables_list(Database *db) {
  if (!db || !db->tables) {
    printf("No database connection\n");
    return;
  }

  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (!hashmap_iterator_has_next(iterator)) {
    printf("No tables found.\n");
    hashmap_iterator_free(iterator);
    return;
  }

  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);
    if (bucket && bucket->value) {
      Table *table = bucket->value;
      printf("%s\n", table->name);
    }
  }

  hashmap_iterator_free(iterator);
}

void database_schema(Database *db) {
  if (db == NULL || db->tables == NULL) {
    printf("No database connection.\n");
    return;
  }

  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (iterator == NULL || !hashmap_iterator_has_next(iterator)) {
    printf("No tables found.\n");
    if (iterator)
      hashmap_iterator_free(iterator);
    return;
  }

  while (hashmap_iterator_has_next(iterator)) {
    Bucket *bucket = hashmap_iterator_next(iterator);
    if (bucket && bucket->value) {
      Table *table = (Table *)bucket->value;
      printf("CREATE TABLE %s (\n", table->name);
      if (table->columns && table->num_columns > 0) {
        for (uint32_t i = 0; i < table->num_columns; i++) {
          Column *col = &table->columns[i];
          const char *type_str = data_type_to_string(col->type);
          if (i < table->num_columns - 1) {
            printf("    %s %s,\n", col->name, type_str);
          } else {
            printf("    %s %s\n", col->name, type_str);
          }
        }
      }
      printf(");\n\n");
    }
  }
  hashmap_iterator_free(iterator);
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
