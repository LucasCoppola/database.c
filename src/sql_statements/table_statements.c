#include <stdio.h>
#include <string.h>

#include "../include/database.h"
#include "../include/sql_statements.h"

PrepareResult prepare_table_statement(char *command, SQLStatement *statement) {
  if (strncmp(command, "CREATE TABLE", 12) == 0) {
    statement->table_type = TABLE_STATEMENT_CREATE;
    int args = sscanf(command, "CREATE TABLE %s", statement->table_name);
    if (args < 1) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }

  if (strncmp(command, "SELECT", 6) == 0) {
    statement->table_type = TABLE_STATEMENT_SELECT;
    int args = sscanf(command, "SELECT * FROM %s", statement->table_name);
    if (args < 1) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_table_statement(Database *db, SQLStatement *statement) {
  Table *table;

  switch (statement->table_type) {
  case TABLE_STATEMENT_CREATE:
    // create_table(db, statement->table_name);
    printf("Table '%s' created successfully.\n", statement->table_name);
    break;

  case TABLE_STATEMENT_SELECT:
    table = find_table(db, statement->table_name);
    if (table) {
      // select_from_table(table);
    } else {
      printf("Table '%s' does not exist.\n", statement->table_name);
    }
    break;
  }
}

// void create_table_cmd(Database *db, char *command) {
//   char table_name[256];
//   if (sscanf(command, "CREATE TABLE %s", table_name) == 1) {
//     create_table(db, table_name);
//     printf("Table '%s' created successfully.\n", table_name);
//   } else {
//     printf("Invalid CREATE TABLE syntax.\n");
//   }
// }
