#ifndef DATABASE_H
#define DATABASE_H

#define MAX_NAME_LENGTH 64

typedef struct Row {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
    struct Row* next; 
} Row;

typedef struct Table {
    char name[MAX_NAME_LENGTH];
    Row* rows_head;
    Row* rows_tail;
    size_t size;
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

#endif
