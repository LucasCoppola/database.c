#include <stdio.h>
#include <string.h>

#include "../include/database.h"
#include "../include/sql_statements.h"

// void create_table_cmd(Database *db, char *command) {
//   char table_name[256];
//   if (sscanf(command, "CREATE TABLE %s", table_name) == 1) {
//     create_table(db, table_name);
//     printf("Table '%s' created successfully.\n", table_name);
//   } else {
//     printf("Invalid CREATE TABLE syntax.\n");
//   }
// }
//
// void select_rows_cmd(Database *db, char *command) {
//   char table_name[256], name[256];
//   int age;
//   sscanf(command, "INSERT INTO %s (name, age) VALUES (%s, %d)", table_name,
//          name, &age);
//   Table *table = find_table(db, table_name);
//   if (table) {
//     insert_row(table, name, age);
//   } else {
//     printf("Table %s does not exist\n", table_name);
//   }
// }
//
// void insert_rows_cmd(Database *db, char *command) {
//   char table_name[256];
//   sscanf(command, "SELECT * FROM %s", table_name);
//   Table *table = find_table(db, table_name);
//   if (table) {
//     select_from_table(table);
//   } else {
//     printf("Table %s does not exist\n", table_name);
//   }
// }
//
// void delete_rows_cmd(Database *db, char *command) {
//   char table_name[256];
//   sscanf(command, "SELECT * FROM %s", table_name);
//   Table *table = find_table(db, table_name);
//   if (table) {
//     select_from_table(table);
//   } else {
//     printf("Table %s does not exist\n", table_name);
//   }
// }

PrepareResult prepare_statement(char *command, Statement *statement) {
  if (strncmp(command, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  } else if (strncmp(command, "select", 6) == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Database *db, Statement *statement) {
  switch (statement->type) {
  case (STATEMENT_INSERT):
    printf("This is where we would do an insert.\n");
    break;
  case (STATEMENT_SELECT):
    printf("This is where we would do a select.\n");
    break;
  }
}
