#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../include/database.h"
#include "../include/statements.h"
#include "../include/storage.h"

void to_lowercase(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = tolower((unsigned char)str[i]);
  }
}

PrepareResult prepare_statement(char *command, Statement *statement) {
  to_lowercase(command);

  if (strncmp(command, "create table", 12) == 0) {
    statement->type = STATEMENT_CREATE_TABLE;
    int args = sscanf(command, "create table %s", statement->table_name);
    return (args < 1) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
  }

  if (strncmp(command, "select", 6) == 0) {
    statement->type = STATEMENT_SELECT;
    int args = sscanf(command, "select * from %s", statement->table_name);
    return (args < 1) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
  }

  if (strncmp(command, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    int args = sscanf(
        command, "insert into %s (username, email) values (%s, %s)",
        statement->table_name, statement->row.username, statement->row.email);
    return (args < 2) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
  }

  if (strncmp(command, "delete", 6) == 0) {
    statement->type = STATEMENT_DELETE;
    int args = sscanf(command, "delete from %s where id = %d",
                      statement->table_name, &statement->row.id);
    return (args < 2) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

static ExecuteResult execute_create_table(Database *db, Statement *statement) {
  Table *table = create_table(db, statement->table_name);
  if (table == NULL) {
    printf("Failed to create table '%s'\n", statement->table_name);
    return EXECUTE_FAILURE;
  }
  printf("Table '%s' created successfully.\n", statement->table_name);
  return EXECUTE_SUCCESS;
}

static ExecuteResult execute_select(Database *db, Statement *statement) {
  Table *table = find_table(db, statement->table_name);
  if (!table) {
    printf("Table '%s' does not exist.\n", statement->table_name);
    return EXECUTE_FAILURE;
  }

  Row row;
  for (uint32_t i = 0; i < table->num_rows; i++) {
    deserialize_row(get_row_slot(table, i), &row);
    printf("(%d, %s, %s)\n", row.id, row.username, row.email);
  }
  return EXECUTE_SUCCESS;
}

static ExecuteResult execute_insert(Database *db, Statement *statement) {
  Table *table = find_table(db, statement->table_name);
  if (!table) {
    printf("Table '%s' does not exist.\n", statement->table_name);
    return EXECUTE_FAILURE;
  }

  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }

  serialize_row(&statement->row, get_row_slot(table, table->num_rows));
  table->num_rows += 1;

  printf("Inserted row into '%s' table.\n", statement->table_name);
  return EXECUTE_SUCCESS;
}

// void insert_row(Table *table, char *name, int age) {
//   if (strlen(name) >= MAX_NAME_LENGTH) {
//     printf("Row name is too long (max %d characters)\n", MAX_NAME_LENGTH -
//     1); free(row); return;
//   }
//   strncpy(row->name, name, MAX_NAME_LENGTH - 1);
//   row->name[MAX_NAME_LENGTH - 1] = '\0';
//
//   row->id = table->next_id;
//   row->age = age;
//
//   table->size++;
// };

// static ExecuteResult execute_delete(Database *db, Statement *statement) {
//   Table *table = find_table(db, statement->table_name);
//   if (!table) {
//     printf("Table '%s' does not exist.\n", statement->table_name);
//     return EXECUTE_FAILURE;
//   }
//
//   printf("Deleted row with id %d from '%s' table.\n", statement->row.id,
//          statement->table_name);
//   return EXECUTE_SUCCESS;
// }

ExecuteResult execute_statement(Database *db, Statement *statement) {
  switch (statement->type) {
  case STATEMENT_CREATE_TABLE:
    return execute_create_table(db, statement);
  case STATEMENT_SELECT:
    return execute_select(db, statement);
  case STATEMENT_INSERT:
    return execute_insert(db, statement);
  case STATEMENT_DELETE:
    return printf("Delete not implemented yet.\n");
    // return execute_delete(db, statement);
  default:
    return EXECUTE_FAILURE;
  }
}
