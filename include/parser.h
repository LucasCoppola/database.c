#ifndef PARSER_H
#define PARSER_H

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

#include "tokenizer.h"

// parser.c
void* parse(const Token* tokens, int token_count);

// statements/
ASTNode* parser_table_create(const Token* tokens, int token_count);
ASTNode* parser_table_drop(const Token* tokens);
Column *parse_columns(const Token *tokens, int token_count, int *index, int *num_columns);

ASTNode* parser_row_select(const Token* tokens, int token_count);
ASTNode* parser_row_insert(const Token* tokens, int token_count);
ASTNode* parser_row_delete(const Token* tokens, int token_count);

// parser_utils.c
bool expect_token(const Token *tokens, int index, TokenType type, const char *value);
DataType map_column_type(char *type);

// ast.c
ASTNode *create_ast_node(NodeType type);
void ast_free(ASTNode* node);


#endif
