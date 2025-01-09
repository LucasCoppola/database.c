#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../include/database.h"
#include "../include/statements.h"
#include "../include/storage.h"
#include "error.h"

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
        command, "insert into %s (username, email) values (%[^,], %[^)])",
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
  Table *out_table = NULL;
  TableResult table_result =
      create_table(db, statement->table_name, &out_table);

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", table_result);
    return EXECUTE_FAILURE;
  }

  printf("Table '%s' created successfully.\n", statement->table_name);
  return EXECUTE_SUCCESS;
}

static ExecuteResult execute_select(Database *db, Statement *statement) {
  Table *out_table = NULL;
  TableResult table_result = find_table(db, statement->table_name, &out_table);

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", table_result);
    return EXECUTE_FAILURE;
  }

  Row row;
  for (uint32_t i = 0; i < out_table->num_rows; i++) {
    deserialize_row(get_row_slot(out_table, i), &row);
    printf("(%d, %s, %s)\n", row.id, row.username, row.email);
  }
  return EXECUTE_SUCCESS;
}

void insert_row(Table *table, Row row) {
  if (strlen(row.username) >= MAX_NAME_LENGTH ||
      strlen(row.email) >= MAX_NAME_LENGTH) {
    printf("name is too long (max %d characters)\n", MAX_NAME_LENGTH - 1);
    return;
  }

  void *slot = get_row_slot(table, table->num_rows);
  row.id = table->next_id++;
  serialize_row(&row, slot);
  table->num_rows++;
};

static ExecuteResult execute_insert(Database *db, Statement *statement) {
  Table *out_table = NULL;
  TableResult table_result = find_table(db, statement->table_name, &out_table);

  if (table_result != TABLE_SUCCESS) {
    LOG_ERROR("table", table_result);
    return EXECUTE_FAILURE;
  }

  if (out_table->num_rows >= TABLE_MAX_ROWS) {
    printf("Error: Table '%s' is full.\n", statement->table_name);
    return EXECUTE_TABLE_FULL;
  }

  insert_row(out_table, statement->row);
  printf("Inserted row into '%s' table.\n", statement->table_name);
  return EXECUTE_SUCCESS;
}

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
