#ifndef AST_H
#define AST_H

#include <stdbool.h>

typedef struct Column Column;
typedef struct Value Value;

typedef enum ASTNodeResult {
    AST_SUCCESS,
    AST_ALLOC_ERROR
} ASTNodeResult;

typedef enum {
    NODE_CREATE_TABLE,
    NODE_DROP_TABLE,
    NODE_SELECT,
    NODE_INSERT,
    NODE_DELETE
} NodeType;

typedef struct WhereCondition {
    char *column_name;          
    char op;                 
    Value* value;           
} WhereCondition;

typedef struct {
    Column* columns;
    int num_columns;
} CreateTable;

typedef struct {
    Value** values;
    int num_values;
} InsertRows;

typedef struct {
    char** select_columns;
    int num_columns;
    bool select_all;
} SelectRows;

typedef struct ASTNode {
    NodeType type;
    char* table_name;
    CreateTable create_table;
    InsertRows insert_rows;
    SelectRows select_rows;
    WhereCondition where_condition;   
} ASTNode;

ASTNodeResult create_ast_node(NodeType type, ASTNode **out_node);
void ast_free(ASTNode* node);
void ast_free_where_condition(WhereCondition where_condition);

#endif 
