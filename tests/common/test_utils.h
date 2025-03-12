#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "core/table.h"
#include "parser/tokenizer.h"

TokenizerState *setup_tokenizer(const char *query);
DataType parse_data_type(const char *type);

#endif
