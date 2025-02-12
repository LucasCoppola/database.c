#ifndef EXECUTOR_H
#define EXECUTOR_H

typedef struct ASTNode ASTNode;

typedef enum { 
  EXECUTE_SUCCESS, 
  EXECUTE_FAILURE, 
  EXECUTE_TABLE_FULL,
} ExecuteResult;

ExecuteResult execute_ast(Database *db, ASTNode *node);

ExecuteResult execute_create_table(Database *db, ASTNode *node);

#endif
