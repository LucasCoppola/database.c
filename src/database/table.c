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

  off_t min_data_offset = sizeof(uint32_t) + (MAX_TABLES * HEADER_SIZE);
  off_t current_data_position = min_data_offset;

  // Calculate data offsets for existing tables
  HashMapIterator *iterator = hashmap_iterator_init(db->tables);
  if (iterator != NULL) {
    while (hashmap_iterator_has_next(iterator)) {
      printf("Current data position after creating table: %ld\n",
             current_data_position);
      Bucket *bucket = hashmap_iterator_next(iterator);
      if (bucket != NULL) {
        Table *existing_table = bucket->value;
        uint32_t num_pages =
            (existing_table->num_rows + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
        if (num_pages == 0)
          num_pages = 1; // Always allocate at least one page
        current_data_position += num_pages * PAGE_SIZE;
      }
    }
    hashmap_iterator_free(iterator);
  }

  // Initialize the table
  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';
  table->next_id = 1;
  table->num_rows = 0;
  table->pager = db->pager;
  table->page_offset = current_data_position;
  memset(table->pages, 0, sizeof(void *) * TABLE_MAX_PAGES);

  printf("Created table %s with initial page_offset: %ld\n", name,
         current_data_position);

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
    if (table->pages[i] == NULL) {
      continue;
    }
    PagerResult result = pager_pages_flush(pager, table);
    if (result != PAGER_SUCCESS) {
      LOG_ERROR("pager", result);
      return;
    }
    free(table->pages[i]);
    table->pages[i] = NULL;
  }

  // There may be a partial page to write to the end of the file
  // This should not be needed after we switch to a B-tree
  uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
  if (num_additional_rows > 0) {
    uint32_t page_num = num_full_pages;
    if (table->pages[page_num] != NULL) {
      PagerResult result = pager_page_flush(pager, page_num, table);
      if (result != PAGER_SUCCESS) {
        LOG_ERROR("pager", result);
        return;
      }
      free(table->pages[page_num]);
      table->pages[page_num] = NULL;
    }
  }

  int result = close(pager->file_descriptor);
  if (result == -1) {
    printf("Error closing db file.\n");
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    void *page = table->pages[i];
    if (page) {
      free(page);
      table->pages[i] = NULL;
    }
  }
}
