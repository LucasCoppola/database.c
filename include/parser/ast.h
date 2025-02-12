#ifndef AST_H
#define AST_H

#include <stdbool.h>

#define MAX_COLUMNS 10

typedef struct Column Column;

typedef enum {
    NODE_CREATE_TABLE,
    NODE_DROP_TABLE,
    NODE_SELECT,
    NODE_INSERT,
    NODE_DELETE
} NodeType;

typedef struct {
    Column* columns;
    int num_columns;
} CreateTable;

typedef struct {
    char** values;  // Array of string values to be converted later
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
} ASTNode;

ASTNode *create_ast_node(NodeType type);
void ast_free(ASTNode* node);

#endif 
