#include <stdio.h>
#include <string.h>

#include "../include/database.h"
#include "../include/sql_statements.h"

PrepareResult prepare_row_statement(char *command, SQLStatement *statement) {
  if (strncmp(command, "INSERT", 6) == 0) {
    statement->row_type = ROW_STATEMENT_INSERT;
    int args = sscanf(
        command, "INSERT INTO %s (username, email) VALUES (%s, %s)",
        statement->table_name, statement->row.username, statement->row.email);
    if (args < 3) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }

  if (strncmp(command, "DELETE", 6) == 0) {
    statement->row_type = ROW_STATEMENT_DELETE;
    int args = sscanf(command, "DELETE FROM %s WHERE id = %d",
                      statement->table_name, &statement->row.id);
    if (args < 2) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_row_statement(Database *db, SQLStatement *statement) {
  Table *table = find_table(db, statement->table_name);
  if (!table) {
    printf("Table '%s' does not exist.\n", statement->table_name);
    return;
  }

  switch (statement->row_type) {
  case ROW_STATEMENT_INSERT:
    // insert_row(table, statement->row.username, statement->row.email);
    printf("Inserted row into '%s' table.\n", statement->table_name);
    break;

  case ROW_STATEMENT_DELETE:
    // delete_row(table, statement->row.id);
    printf("Deleted row with id %d from '%s' table.\n", statement->row.id,
           statement->table_name);
    break;
  }
}

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
