#ifndef SQL_STATEMENTS_H
#define SQL_STATEMENTS_H

#include <stdio.h>

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef struct {
  StatementType type;
} Statement;

PrepareResult prepare_statement(char *command, Statement *statement);
void execute_statement(Database *db, Statement *statement);

#endif
