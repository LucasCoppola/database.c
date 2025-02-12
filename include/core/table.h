#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <stdbool.h>

#include "storage/table_header.h"

#define TABLE_MAX_PAGES 100
#define MAX_TABLES 50

typedef struct Database Database;  
typedef struct Pager Pager;

typedef enum TableResult {
    TABLE_SUCCESS,
    TABLE_NOT_FOUND,
    TABLE_ALLOC_ERROR,
    TABLE_NAME_TOO_LONG,
    TABLE_INVALID_DB,
    TABLE_ALREADY_EXISTS,
    TABLE_HASHMAP_SET_ERROR
} TableResult;

typedef enum {
    TYPE_INT,
    TYPE_TEXT,
    TYPE_UNKNOWN
} DataType;

typedef struct Column {
    char* name;
    DataType type;
} Column;

typedef struct Table {
    uint32_t next_id; 
    uint32_t num_rows;
    uint32_t page_offset;
    char name[MAX_NAME_LENGTH];
    Pager *pager;
    void* pages[TABLE_MAX_PAGES];  
} Table;

// table.c
TableResult table_create(Database* db, char* name, Table **out_table);
TableResult table_find(Database* db, char* name, Table **out_table);
TableResult table_drop(Database *db, char *name);

// table_utils.c
TableResult table_initialize(Table *table, char *name, Database *db);
off_t table_position(Database *db);
uint32_t table_pages_count(Table *table);

#endif
