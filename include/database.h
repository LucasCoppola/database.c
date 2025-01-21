#ifndef DATABASE_H
#define DATABASE_H

#define MAX_NAME_LENGTH 64
#define TABLE_MAX_PAGES 100
#define MAX_TABLES 50
#define PAGE_SIZE 4096

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "hashmap.h"

typedef enum {
  DATABASE_ALLOC_ERROR,
  DATABASE_HASHMAP_INIT_ERROR,
  DATABASE_PAGER_INIT_ERROR,
  DATABASE_SUCCESS,
} DatabaseResult;

typedef enum {
  TABLE_SUCCESS,
  TABLE_NOT_FOUND,
  TABLE_ALLOC_ERROR,
  TABLE_NAME_TOO_LONG,
  TABLE_INVALID_DB,
  TABLE_ALREADY_EXISTS,
  TABLE_HASHMAP_SET_ERROR
} TableResult;

typedef enum {
    ROW_SUCCESS,
    ROW_NAME_TOO_LONG,
    ROW_INVALID_TABLE,
    ROW_NOT_FOUND,
    ROW_ALLOC_PAGE_ERROR,
    ROW_GET_PAGE_ERROR,
    ROW_FLUSH_PAGE_ERROR,
} RowResult;

typedef struct {
    uint32_t id;
    char username[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
} Row;

typedef struct {
  int file_descriptor;
  uint32_t file_length;
  uint32_t num_pages;
  uint32_t num_tables;
} Pager;

typedef struct Table {
    uint32_t next_id; // id of the next row to be inserted
    uint32_t num_rows;
    uint32_t page_offset;
    char name[MAX_NAME_LENGTH];
    Pager *pager;
    void* pages[TABLE_MAX_PAGES];  
} Table;

typedef struct {
  Table* table;
  uint32_t row_num;
  bool end_of_table;
} Cursor;

typedef struct Database {
    HashMap *tables;
    Pager *pager;
} Database;

DatabaseResult database_open(Database **out_db, const char *filename);
void database_close(Database *db);
void database_free(Database *db);


TableResult create_table(Database* db, char* name, Table **out_table);
TableResult find_table(Database* db, char* name, Table **out_table);
TableResult drop_table(Database *db, char *name);
void close_table(Table *table, Pager *pager);
void free_table(Table *table);

RowResult insert_row(Table *table, Row *row);
RowResult select_rows(Table *table);
RowResult delete_row(Table *table, uint32_t row_id);

#endif
