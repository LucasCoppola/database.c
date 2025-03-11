#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "core/table.h"
#include "parser/tokenizer.h"

extern char *current_query;
extern TokenizerState *state;
extern ASTNode *node;

TokenizerState *setup_tokenizer(const char *query);
void teardown_tokenizer(TokenizerState *state);
DataType parse_data_type(const char *type);

void setUp(void);
void tearDown(void);

#endif
