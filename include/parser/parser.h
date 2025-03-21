#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "core/table.h"

typedef struct ASTNode ASTNode;

// parser.c
ASTNode* parse(const Token* tokens, int token_count);

// parser_utils.c
bool expect_token(const Token *tokens, int index, TokenType type, const char *value);
DataType map_column_type(char *type);

#endif
