#ifndef QUERY_PARSER_H
#define QUERY_PARSER_H

#define MAX_VALUE_LENGTH 256

typedef enum {
  TOKENIZER_SUCCESS,
  TOKENIZER_ERROR
} TokenizerResult;

typedef enum {
    TOKEN_KEYWORD,     // Reserved SQL keywords like SELECT, INSERT
    TOKEN_IDENTIFIER,  // Table/column names
    TOKEN_LITERAL,     // String, number, or boolean values
    TOKEN_OPERATOR,    // Operators like =, >, <
    TOKEN_PUNCTUATION, // Symbols like commas, parentheses
    TOKEN_WILDCARD,    // *
    TOKEN_EOF          // End of input
} TokenType;

typedef struct {
    TokenType type;  
    char value[MAX_VALUE_LENGTH];  
    int position;      
} Token;

typedef struct {
    char* query;   
    int position; 
    int length;  
    Token *tokens;
    int token_count;
} TokenizerState;

// tokenizer.c
TokenizerState *tokenizer_init(const char* query);
TokenizerResult tokenize_query(TokenizerState* state);
void tokenizer_free(TokenizerState* state);

// tokenizer_utils.c
void add_token(TokenizerState* state, const char* value, TokenType type, int position);

char *read_word(char* query, int* position);
char *read_numeric_literal(char *query, int *position);
char *read_string_literal(char *query, int *position, char quote);

bool is_keyword(char* value);
bool is_operator(char value);
bool is_punctuation(char value);

#endif
