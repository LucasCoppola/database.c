// #ifndef STMT_H
// #define STMT_H
//
// #include <stdio.h>
// #include "database.h"
//
// typedef enum { 
//   PREPARE_SUCCESS,
//   PREPARE_UNRECOGNIZED_STATEMENT, 
//   PREPARE_SYNTAX_ERROR
// } PrepareResult;
//
// typedef enum { 
//   EXECUTE_SUCCESS, 
//   EXECUTE_FAILURE, 
//   EXECUTE_TABLE_FULL,
// } ExecuteResult;
//
// typedef enum {
//     STATEMENT_CREATE_TABLE,
//     STATEMENT_DROP_TABLE,
//     STATEMENT_SELECT,
//     STATEMENT_INSERT,
//     STATEMENT_DELETE,
// } StatementType;
//
// typedef struct {
//   char table_name[MAX_NAME_LENGTH];
//   StatementType type;
//   Row row;  // Used for INSERT/DELETE operations
// } Statement;
//
// PrepareResult prepare_statement(char *command, Statement *stmt);
// ExecuteResult execute_statement(Database *db, Statement *stmt);
//
// #endif
