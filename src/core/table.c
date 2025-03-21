#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "core/database.h"
#include "core/table.h"

#include "core/hashmap.h"
#include "parser/ast.h"
#include "parser/statements.h"
#include "storage/pager.h"
#include "utils/logger.h"

TableResult table_create(Database *db, ASTNode *node, Table **out_table) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  Table *table = malloc(sizeof(Table));
  if (table == NULL) {
    return TABLE_ALLOC_ERROR;
  }

  TableResult init_result = table_initialize(table, node->table_name, db);
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

  HashMapResult map_result = hashmap_set(db->tables, node->table_name, table);
  if (map_result != HASHMAP_SUCCESS) {
    DEBUG_LOG("hashmap", "set", map_result);
    table_free(table);
    return TABLE_CREATE_ERROR;
  }

  *out_table = table;
  return TABLE_SUCCESS;
}

TableResult table_find(Database *db, char *table_name, Table **out_table) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  Table *table = NULL;
  if (hashmap_get(db->tables, table_name, &table) == HASHMAP_SUCCESS) {
    *out_table = table;
    return TABLE_SUCCESS;
  }

  return TABLE_NOT_FOUND;
}

TableResult table_drop(Database *db, char *table_name) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  Table *table = NULL;
  if (hashmap_get(db->tables, table_name, &table) != HASHMAP_SUCCESS) {
    return TABLE_NOT_FOUND;
  }

  HashMapResult delete_result = hashmap_delete(db->tables, table->name);
  if (delete_result != HASHMAP_SUCCESS) {
    DEBUG_LOG("hashmap", "delete", delete_result);
    return TABLE_DELETE_ERROR;
  }

  db->pager->num_tables--;
  table_free(table);
  return TABLE_SUCCESS;
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
