#ifndef DATABASE_H
#define DATABASE_H

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
#define MAX_NAME_LENGTH 64
#define MAX_PAGES 100
#define PAGE_SIZE 4096

#include <stdint.h>


typedef struct Row {
    uint32_t id;
    char username[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
} Row;

typedef struct Table {
    uint32_t next_id; // id of the next row to be inserted
    char name[MAX_NAME_LENGTH];
    uint32_t num_rows;
    void *pages[MAX_PAGES];
    struct Table* next;
} Table;

typedef struct Database {
    Table* tables_head; 
} Database;

Database* create_database();
Table* create_table(Database* db, char* name);
Table* find_table(Database* db, char* name);
void insert_row(Table* table, char* name, int age);
void select_from_table(Table* table);
void delete_row(Table* table, int id);

void free_rows(Row *row);
void free_tables(Table *table);
void free_database(Database *db);

#endif
