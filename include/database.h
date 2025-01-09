#ifndef DATABASE_H
#define DATABASE_H

#define MAX_NAME_LENGTH 64
#define MAX_PAGES 100
#define PAGE_SIZE 4096

#include <stdint.h>
#include "hashmap.h"

typedef enum {
  DATABASE_ALLOC_ERROR,
  DATABASE_HASHMAP_INIT_ERROR,
  DATABASE_SUCCESS,
} DatabaseResult;

typedef enum {
  TABLE_SUCCESS,
  TABLE_NOT_FOUND,
  TABLE_ALLOC_ERROR,
  TABLE_NAME_TOO_LONG,
  TABLE_INVALID_DB,
  TABLE_HASHMAP_SET_ERROR
} TableResult;

typedef enum {
    ROW_SUCCESS,
    ROW_NAME_TOO_LONG,
    ROW_INVALID_TABLE,
    ROW_NOT_FOUND,
} RowResult;

typedef struct Row {
    uint32_t id;
    char username[MAX_NAME_LENGTH + 1];
    char email[MAX_NAME_LENGTH + 1];
} Row;

typedef struct Table {
    uint32_t next_id; // id of the next row to be inserted
    uint32_t num_rows;
    char name[MAX_NAME_LENGTH + 1];
    void *pages[MAX_PAGES];
} Table;

typedef struct Database {
    HashMap *tables;
} Database;

DatabaseResult create_database(Database **out_db);
void free_database(Database *db);

TableResult create_table(Database* db, char* name, Table **out_table);
TableResult find_table(Database* db, char* name, Table **out_table);
TableResult drop_table(Database *db, char *name);
void free_table(Table *table);

RowResult insert_row(Table *table, Row row);
RowResult select_rows(Table *table);
RowResult delete_row(Table *table, uint32_t row_id);

#endif
