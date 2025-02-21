#ifndef EXECUTOR_H
#define EXECUTOR_H

typedef struct ASTNode ASTNode;

typedef enum { 
  EXECUTE_SUCCESS, 
  EXECUTE_FAILURE, 
} ExecuteResult;

ExecuteResult execute_ast(Database *db, ASTNode *node);

ExecuteResult execute_create_table(Database *db, ASTNode *node);
ExecuteResult execute_drop_table(Database *db, ASTNode *node);
ExecuteResult execute_insert_rows(Database *db, ASTNode *node);
ExecuteResult execute_select_rows(Database *db, ASTNode *node);

#endif
