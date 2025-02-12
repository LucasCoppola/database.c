// #include <ctype.h>
// #include <stdio.h>
// #include <string.h>
//
// #include "core/database.h"
// #include "storage.h"
// #include "utils/logger.h"
//
// void to_lowercase(char *str) {
//   for (int i = 0; str[i]; i++) {
//     str[i] = tolower((unsigned char)str[i]);
//   }
// }
//
// PrepareResult prepare_statement(char *command, Statement *statement) {
//   to_lowercase(command);
//
//   if (strncmp(command, "create table", 12) == 0) {
//     statement->type = STATEMENT_CREATE_TABLE;
//     int args = sscanf(command, "create table %s", statement->table_name);
//     return (args < 1) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
//   }
//
//   if (strncmp(command, "drop table", 10) == 0) {
//     statement->type = STATEMENT_DROP_TABLE;
//     int args = sscanf(command, "drop table %s", statement->table_name);
//     return (args < 1) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
//   }
//
//   if (strncmp(command, "select", 6) == 0) {
//     statement->type = STATEMENT_SELECT;
//     int args = sscanf(command, "select * from %s", statement->table_name);
//     return (args < 1) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
//   }
//
//   if (strncmp(command, "insert", 6) == 0) {
//     statement->type = STATEMENT_INSERT;
//     int args =
//         sscanf(command, "insert into %s (%[^,], %[^)])",
//         statement->table_name,
//                statement->row.username, statement->row.email);
//     return (args < 2) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
//   }
//
//   if (strncmp(command, "delete", 6) == 0) {
//     statement->type = STATEMENT_DELETE;
//     int args = sscanf(command, "delete from %s where id = %d",
//                       statement->table_name, &statement->row.id);
//     return (args < 2) ? PREPARE_SYNTAX_ERROR : PREPARE_SUCCESS;
//   }
//
//   return PREPARE_UNRECOGNIZED_STATEMENT;
// }
//
// static ExecuteResult execute_create_table(Database *db, Statement *statement)
// {
//   Table *out_table = NULL;
//   TableResult table_result =
//       table_create(db, statement->table_name, &out_table);
//
//   if (table_result != TABLE_SUCCESS) {
//     LOG_ERROR("table", table_result);
//     return EXECUTE_FAILURE;
//   }
//
//   return EXECUTE_SUCCESS;
// }
//
// static ExecuteResult execute_drop_table(Database *db, Statement *statement) {
//   TableResult table_result = table_drop(db, statement->table_name);
//   if (table_result != TABLE_SUCCESS) {
//     LOG_ERROR("table", table_result);
//     return EXECUTE_FAILURE;
//   }
//
//   return EXECUTE_SUCCESS;
// }
//
// static ExecuteResult execute_select(Database *db, Statement *statement) {
//   Table *out_table = NULL;
//   TableResult table_result = table_find(db, statement->table_name,
//   &out_table);
//
//   if (table_result != TABLE_SUCCESS) {
//     LOG_ERROR("table", table_result);
//     return EXECUTE_FAILURE;
//   }
//
//   RowResult row_result = select_rows(out_table);
//   if (row_result != ROW_SUCCESS) {
//     LOG_ERROR("row", row_result);
//     return EXECUTE_FAILURE;
//   }
//
//   return EXECUTE_SUCCESS;
// }
//
// static ExecuteResult execute_insert(Database *db, Statement *statement) {
//   Table *out_table = NULL;
//   TableResult table_result = table_find(db, statement->table_name,
//   &out_table);
//
//   if (table_result != TABLE_SUCCESS) {
//     LOG_ERROR("table", table_result);
//     return EXECUTE_FAILURE;
//   }
//
//   if (out_table->num_rows >= TABLE_MAX_ROWS) {
//     return EXECUTE_TABLE_FULL;
//   }
//
//   RowResult row_result = insert_row(out_table, &statement->row);
//   if (row_result != ROW_SUCCESS) {
//     LOG_ERROR("row", row_result);
//     return EXECUTE_FAILURE;
//   }
//
//   return EXECUTE_SUCCESS;
// }
//
// static ExecuteResult execute_delete(Database *db, Statement *statement) {
//   Table *out_table = NULL;
//   TableResult table_result = table_find(db, statement->table_name,
//   &out_table);
//
//   if (table_result != TABLE_SUCCESS) {
//     LOG_ERROR("table", table_result);
//     return EXECUTE_FAILURE;
//   }
//
//   RowResult row_result = delete_row(out_table, statement->row.id);
//   if (row_result != ROW_SUCCESS) {
//     LOG_ERROR("row", row_result);
//     return EXECUTE_FAILURE;
//   }
//
//   return EXECUTE_SUCCESS;
// }
//
// ExecuteResult execute_statement(Database *db, Statement *statement) {
//   switch (statement->type) {
//   case STATEMENT_CREATE_TABLE:
//     return execute_create_table(db, statement);
//   case STATEMENT_DROP_TABLE:
//     return execute_drop_table(db, statement);
//   case STATEMENT_SELECT:
//     return execute_select(db, statement);
//   case STATEMENT_INSERT:
//     return execute_insert(db, statement);
//   case STATEMENT_DELETE:
//     return execute_delete(db, statement);
//   default:
//     return EXECUTE_FAILURE;
//   }
// }
