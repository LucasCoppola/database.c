#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum TokenizerResult {
    TOKENIZER_SUCCESS,
    TOKENIZER_STATE_ALLOC_ERROR,
    TOKENIZER_QUERY_ALLOC_ERROR,
    TOKENIZER_TOKENS_ALLOC_ERROR,
    TOKENIZER_READ_WORD_ERROR,
    TOKENIZER_READ_NUMBER_ERROR,
    TOKENIZER_READ_STRING_ERROR,
    TOKENIZER_UNTERMINATED_STATEMENT
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

typedef struct Token {
    char *value;  
    TokenType type;  
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
TokenizerResult tokenizer_init(const char* query, TokenizerState** state);
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
