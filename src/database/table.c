#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/logger.h"

TableResult create_table(Database *db, char *name, Table **out_table) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  Table *table = malloc(sizeof(Table));
  if (table == NULL) {
    return TABLE_ALLOC_ERROR;
  }

  if (strlen(name) >= MAX_NAME_LENGTH) {
    free(table);
    return TABLE_NAME_TOO_LONG;
  }

  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';
  table->next_id = 1;
  table->num_rows = 0;

  HashMapResult map_result = hashmap_set(db->tables, name, table);
  if (map_result != HASHMAP_SUCCESS) {
    LOG_ERROR("hashmap", map_result);
    free(table);
    return TABLE_HASHMAP_SET_ERROR;
  }

  for (uint32_t i = 0; i < MAX_PAGES; i++) {
    table->pages[i] = NULL;
  }

  *out_table = table;
  return TABLE_SUCCESS;
}

TableResult find_table(Database *db, char *name, Table **out_table) {
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

TableResult drop_table(Database *db, char *name) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  if (hashmap_delete(db->tables, name) == HASHMAP_SUCCESS) {
    return TABLE_SUCCESS;
  }

  printf("Table '%s' not found\n", name);
  return TABLE_NOT_FOUND;
}

void free_table(Table *table) {
  if (table == NULL) {
    return;
  }

  for (int i = 0; i < MAX_PAGES; i++) {
    if (table->pages[i] != NULL) {
      free(table->pages[i]);
    }
  }

  free(table);
}
