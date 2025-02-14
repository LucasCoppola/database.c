#ifndef TABLE_HEADER_H
#define TABLE_HEADER_H

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

typedef struct Database Database;  
typedef struct HashMap HashMap;
typedef struct Pager Pager;
typedef struct Table Table;

#define MAX_NAME_LENGTH 64
#define MAX_COLUMNS 10

typedef struct {
    uint32_t next_id;
    uint32_t num_rows;
    uint32_t page_offset;
    uint32_t num_columns;
    char table_name[MAX_NAME_LENGTH];
    struct {
        char name[MAX_NAME_LENGTH];
        uint32_t type;
    } columns[MAX_COLUMNS];
} TableHeader;

#define HEADER_SIZE (sizeof(TableHeader))

// header_restore.c
void header_tables_restore(Pager *pager, HashMap *map);
void header_table_initialize(TableHeader *header, Pager *pager, HashMap *map);
bool header_table_read(Pager *pager, off_t *header_pos, TableHeader *header);
uint32_t header_tables_count_read(int file_descriptor);

// header_store.c
void header_tables_store(Database *db);
void header_tables_write(Database *db);
void header_table_write(Pager *pager, Table *table, size_t idx);
void header_tables_count_write(Pager *pager, uint32_t num_tables);

#endif
