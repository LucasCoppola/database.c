#ifndef DATABASE_H
#define DATABASE_H

#define MAX_NAME_LENGTH 64
#define MAX_PAGES 100
#define PAGE_SIZE 4096

#include <stdint.h>
#include "hashmap.h"

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

Database* create_database();
void free_database(Database *db);

Table* create_table(Database* db, char* name);
Table* find_table(Database* db, char* name);
void free_table(Table *table);

#endif
