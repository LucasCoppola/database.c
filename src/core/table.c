#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "core/database.h"
#include "core/table.h"

#include "storage/pager.h"
#include "utils/hashmap.h"
#include "utils/logger.h"

TableResult table_create(Database *db, char *name, Table **out_table) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  if (hashmap_get(db->tables, name, NULL) == HASHMAP_SUCCESS) {
    return TABLE_ALREADY_EXISTS;
  }

  if (strlen(name) >= MAX_NAME_LENGTH) {
    return TABLE_NAME_TOO_LONG;
  }

  Table *table = malloc(sizeof(Table));
  if (table == NULL) {
    return TABLE_ALLOC_ERROR;
  }

  TableResult result = table_initialize(table, name, db);
  if (result != TABLE_SUCCESS) {
    free(table);
    return result;
  }

  HashMapResult map_result = hashmap_set(db->tables, name, table);
  if (map_result != HASHMAP_SUCCESS) {
    LOG_ERROR("hashmap", map_result);
    free(table);
    return TABLE_HASHMAP_SET_ERROR;
  }

  *out_table = table;
  return TABLE_SUCCESS;
}

TableResult table_find(Database *db, char *name, Table **out_table) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  Table *table = NULL;
  if (hashmap_get(db->tables, name, &table) == HASHMAP_SUCCESS) {
    *out_table = table;
    return TABLE_SUCCESS;
  }

  printf("Table '%s' not found\n", name);
  return TABLE_NOT_FOUND;
}

TableResult table_drop(Database *db, char *name) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  if (hashmap_delete(db->tables, name) == HASHMAP_SUCCESS) {
    db->pager->num_tables--;
    return TABLE_SUCCESS;
  }

  printf("Table '%s' not found\n", name);
  return TABLE_NOT_FOUND;
}
