#ifndef AST_H
#define AST_H

#include <stdbool.h>

#define MAX_COLUMNS 10

typedef enum {
    NODE_CREATE_TABLE,
    NODE_DROP_TABLE,
    NODE_SELECT,
    NODE_INSERT,
    NODE_DELETE
} NodeType;

typedef enum {
    TYPE_INT,
    TYPE_TEXT,
    TYPE_UNKNOWN
} DataType;

typedef struct {
    char* name;
    DataType type;
} Column;

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

typedef struct {
    NodeType type;
    char* table_name;
    CreateTable create_table;
    InsertRows insert_rows;
    SelectRows select_rows;
} ASTNode;

ASTNode *create_ast_node(NodeType type);
void ast_free(ASTNode* node);

#endif 
