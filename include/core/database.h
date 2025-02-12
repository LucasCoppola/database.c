#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct HashMap HashMap;
typedef struct Pager Pager;

typedef enum DatabaseResult {
  DATABASE_ALLOC_ERROR,
  DATABASE_HASHMAP_INIT_ERROR,
  DATABASE_PAGER_INIT_ERROR,
  DATABASE_SUCCESS,
} DatabaseResult;

typedef struct Database {
    HashMap *tables;
    Pager *pager;
} Database;

DatabaseResult database_open(Database **out_db, const char *filename);
void database_close(Database *db);
void database_free(Database *db);

#endif
