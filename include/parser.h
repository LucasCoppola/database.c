#ifndef PARSER_H
#define PARSER_H

typedef enum {
    NODE_CREATE_TABLE,
    NODE_DROP_TABLE,
    NODE_SELECT,
    NODE_INSERT,
    NODE_DELETE
} NodeType;

typedef enum {
    TYPE_INT,
    TYPE_TEXT
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
    int num_select_columns;
    bool select_all;
} SelectRows; 

typedef struct {
    NodeType type;
    char* table_name;
    CreateTable create_table;
    InsertRows insert_rows;
    SelectRows select_rows;
} ASTNode;

#include "tokenizer.h"

void* parse(const Token* tokens, int token_count);

ASTNode* parser_table_create(const Token* tokens, int token_count);
ASTNode* parser_table_drop(const Token* tokens, int token_count);

ASTNode* parser_row_select(const Token* tokens, int token_count);
ASTNode* parser_row_insert(const Token* tokens, int token_count);
ASTNode* parser_row_delete(const Token* tokens, int token_count);

void ast_free(ASTNode* node);

#endif
