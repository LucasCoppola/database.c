#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/database.h"

Database *create_database() {
  Database *db = malloc(sizeof(Database));
  if (db == NULL) {
    printf("Failed to allocate db\n");
    return NULL;
  }

  db->tables_head = NULL;
  return db;
}

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
    printf("Table name too long (max %d characters)\n", MAX_NAME_LENGTH - 1);
    free(table);
    return NULL;
  }
  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';

  table->size = 0;
  table->next_id = 1;
  table->rows_head = NULL;
  table->rows_tail = NULL;

  table->next = db->tables_head;
  db->tables_head = table;
  return table;
};

void insert_row(Table *table, char *name, int age) {
  if (table == NULL) {
    printf("Table is null\n");
    return;
  }

  Row *row = malloc(sizeof(Row));
  if (row == NULL) {
    printf("Failed to allocate row\n");
    return;
  }

  if (strlen(name) >= MAX_NAME_LENGTH) {
    printf("Row name too long (max %d characters)\n", MAX_NAME_LENGTH - 1);
    free(row);
    return;
  }
  strncpy(row->name, name, MAX_NAME_LENGTH - 1);
  row->name[MAX_NAME_LENGTH - 1] = '\0';

  row->id = table->next_id;
  row->age = age;
  row->next = NULL;

  if (table->rows_head == NULL) {
    table->rows_head = table->rows_tail = row;
  } else {
    table->rows_tail->next = row;
    table->rows_tail = row;
  }

  table->size++;
};

void select_from_table(Table *table) {
  if (table == NULL) {
    printf("Table is null\n\n");
    return;
  }

  if (table->rows_head == NULL) {
    printf("No rows in the table\n");
    return;
  }

  Row *curr = table->rows_head;
  while (curr) {
    printf("ID: %d, Name: %s, Age: %d\n", curr->id, curr->name, curr->age);
    curr = curr->next;
  }
};

void delete_row(Table *table, int id) {
  if (table == NULL) {
    printf("Table is null\n");
    return;
  }

  Row *curr = table->rows_head;
  Row *prev = NULL;

  while (curr != NULL) {
    if (curr->id == id) {
      if (prev == NULL) {
        table->rows_head = curr->next;
      } else {
        prev->next = curr->next;
      }

      if (curr->next == NULL) {
        table->rows_tail = prev;
      }

      free(curr);
      table->size--;
      return;
    }

    prev = curr;
    curr = curr->next;
  }

  printf("Row with id %d not found\n", id);
};

void free_rows(Row *row) {
  while (row) {
    Row *temp = row;
    row = row->next;
    free(temp);
  }
}

void free_tables(Table *table) {
  while (table) {
    Table *temp = table;
    free_rows(table->rows_head);
    table = table->next;
    free(temp);
  }
}

void free_database(Database *db) {
  if (db) {
    free_tables(db->tables_head);
    free(db);
  }
}
