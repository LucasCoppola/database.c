#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "core/database.h"
#include "core/table.h"

#include "parser/ast.h"
#include "parser/statements.h"
#include "storage/pager.h"
#include "utils/hashmap.h"
#include "utils/logger.h"

TableResult table_create(Database *db, ASTNode *node, Table **out_table) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  char *table_name = node->table_name;
  if (hashmap_get(db->tables, table_name, NULL) == HASHMAP_SUCCESS) {
    return TABLE_ALREADY_EXISTS;
  }

  if (strlen(table_name) >= MAX_NAME_LENGTH) {
    return TABLE_NAME_TOO_LONG;
  }

  Table *table = malloc(sizeof(Table));
  if (table == NULL) {
    return TABLE_ALLOC_ERROR;
  }

  TableResult init_result = table_initialize(table, table_name, db);
  if (init_result != TABLE_SUCCESS) {
    table_free(table);
    return init_result;
  }

  Column *columns = node->create_table.columns;
  int num_columns = node->create_table.num_columns;
  TableResult set_cols_result = table_columns_set(table, columns, num_columns);
  if (set_cols_result != TABLE_SUCCESS) {
    table_free(table);
    return set_cols_result;
  }

  HashMapResult map_result = hashmap_set(db->tables, table_name, table);
  if (map_result != HASHMAP_SUCCESS) {
    LOG_ERROR("hashmap", map_result);
    table_free(table);
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

  Table *table = NULL;
  if (hashmap_get(db->tables, name, &table) == HASHMAP_SUCCESS) {
    if (hashmap_delete(db->tables, name) == HASHMAP_SUCCESS) {
      db->pager->num_tables--;
      table_free(table);
      return TABLE_SUCCESS;
    }
  }

  printf("Table '%s' not found\n", name);
  return TABLE_NOT_FOUND;
}

void table_free(Table *table) {
  if (!table)
    return;

  if (table->columns) {
    for (uint32_t i = 0; i < table->num_columns; i++) {
      free(table->columns[i].name);
    }
    free(table->columns);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    if (table->pages[i]) {
      free(table->pages[i]);
    }
  }

  free(table);
}
