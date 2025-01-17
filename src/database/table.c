#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/logger.h"
#include "../include/storage.h"

TableResult create_table(Database *db, char *name, Table **out_table) {
  if (db == NULL) {
    return TABLE_INVALID_DB;
  }

  if (hashmap_get(db->tables, name, NULL) == HASHMAP_SUCCESS) {
    return TABLE_ALREADY_EXISTS;
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
  table->pager = db->pager;
  table->pager->num_tables++;

  HashMapResult map_result = hashmap_set(db->tables, name, table);
  if (map_result != HASHMAP_SUCCESS) {
    LOG_ERROR("hashmap", map_result);
    free(table);
    return TABLE_HASHMAP_SET_ERROR;
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
    db->pager->num_tables--;
    return TABLE_SUCCESS;
  }

  printf("Table '%s' not found\n", name);
  return TABLE_NOT_FOUND;
}

void free_table(Table *table) {
  if (table == NULL) {
    return;
  }

  free(table);
}

void close_table(Table *table, Pager *pager) {
  uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

  for (uint32_t i = 0; i < num_full_pages; i++) {
    if (pager->pages[i] == NULL) {
      continue;
    }
    pager_flush(pager, i);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  // There may be a partial page to write to the end of the file
  // This should not be needed after we switch to a B-tree
  uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
  if (num_additional_rows > 0) {
    uint32_t page_num = num_full_pages;
    if (pager->pages[page_num] != NULL) {
      pager_flush(pager, page_num);
      free(pager->pages[page_num]);
      pager->pages[page_num] = NULL;
    }
  }

  int result = close(pager->file_descriptor);
  if (result == -1) {
    printf("Error closing db file.\n");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    void *page = pager->pages[i];
    if (page) {
      free(page);
      pager->pages[i] = NULL;
    }
  }
}
