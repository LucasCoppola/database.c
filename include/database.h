#ifndef DATABASE_H
#define DATABASE_H

#define MAX_NAME_LENGTH 64

typedef struct Row {
    int id;
    int age;
    char name[MAX_NAME_LENGTH];
    struct Row* next; 
} Row;

typedef struct Table {
    char name[MAX_NAME_LENGTH];
    // only increases
    int next_id;
    size_t size;
    Row* rows_head;
    Row* rows_tail;
    struct Table* next;
} Table;

typedef struct Database {
    Table* tables_head; 
} Database;

Database* create_database();
Table* create_table(Database* db, char* name);
void insert_row(Table* table, char* name, int age);
void select_from_table(Table* table);
void delete_row(Table* table, int id);

void free_rows(Row *row);
void free_tables(Table *table);
void free_database(Database *db);

#endif
