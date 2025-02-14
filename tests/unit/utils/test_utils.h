#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "core/table.h"

TokenizerState *setup_tokenizer(const char *query);
void teardown_tokenizer(TokenizerState *state);
void print_test_result(const char *test_name, bool passed);
DataType parse_data_type(const char *type);

#endif
