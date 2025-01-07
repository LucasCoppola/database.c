#ifndef SQL_STATEMENTS_H
#define SQL_STATEMENTS_H

#include <stdio.h>
#include "database.h"

typedef enum { 
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED_STATEMENT, 
  PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum { 
  TABLE_STATEMENT_CREATE, 
  TABLE_STATEMENT_SELECT, 
} TableSQLStatement;

typedef enum { 
  ROW_STATEMENT_INSERT, 
  ROW_STATEMENT_DELETE, 
} RowSQLStatement;

typedef struct {
  char table_name[MAX_NAME_LENGTH];
  Row row;
  RowSQLStatement row_type;
  TableSQLStatement table_type;
} SQLStatement;

PrepareResult prepare_table_statement(char *command, SQLStatement *statement);
PrepareResult prepare_row_statement(char *command, SQLStatement *statement);

void execute_table_statement(Database *db, SQLStatement *statement);
void execute_row_statement(Database *db, SQLStatement *statement);

#endif
