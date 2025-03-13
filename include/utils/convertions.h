#ifndef CONVERTIONS_H
#define CONVERTIONS_H

#include "core/row.h"
#include "parser/keywords.h"

DataType map_column_type(char *type);
char *data_type_to_string(DataType type);
Value *convert_value(const char *value_str, DataType type);
void print_value(Value value, DataType type);

#endif 
