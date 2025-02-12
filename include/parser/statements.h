#ifndef STATEMENTS_H
#define STATEMENTS_H

typedef struct ASTNode ASTNode;
typedef struct Token Token;
typedef struct Column Column;

// create_table.c
ASTNode* parser_table_create(const Token* tokens, int token_count);
Column *columns_parse(const Token *tokens, int token_count, int *index, int *num_columns);
void columns_free(Column *columns, int num_columns);

// drop_table.c
ASTNode* parser_table_drop(const Token* tokens);

// select.c
ASTNode* parser_row_select(const Token* tokens, int token_count);

// insert.c
ASTNode* parser_row_insert(const Token* tokens, int token_count);

// delete.c
ASTNode* parser_row_delete(const Token* tokens, int token_count);

#endif 
