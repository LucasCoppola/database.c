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
    TOKEN_EOF          // End of input
} TokenType;

typedef struct {
    TokenType type;  
    char value[256];  
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
bool is_keyword(const char* value);
void add_token(TokenizerState* state, const char* value, TokenType type, int position);
char* read_word(char* query, int* position);
