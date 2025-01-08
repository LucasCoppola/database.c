#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../include/database.h"
#include "../include/hashmap.h"

Table *create_table(Database *db, char *name) {
  if (db == NULL) {
    printf("Database is null\n");
    return NULL;
  }

  Table *table = malloc(sizeof(Table));
  if (table == NULL) {
    printf("Failed to allocate table\n");
    return NULL;
  }

  if (strlen(name) >= MAX_NAME_LENGTH) {
    printf("Table name is too long (max %d characters)\n", MAX_NAME_LENGTH - 1);
    free(table);
    return NULL;
  }
  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';

  table->next_id = 1;
  table->num_rows = 0;

  if (hashmap_set(db->tables, name, table) != HASHMAP_SUCCESS) {
    free(table);
    return NULL;
  }

  for (uint32_t i = 0; i < MAX_PAGES; i++) {
    table->pages[i] = NULL;
  }

  return table;
};

Table *find_table(Database *db, char *name) {
  if (db == NULL) {
    printf("Database is null\n");
    return NULL;
  }

  Table *table = NULL;
  if (hashmap_get(db->tables, name, (Table **)&table) == HASHMAP_SUCCESS) {
    return table;
  }

  return NULL;
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
