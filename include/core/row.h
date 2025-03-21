#ifndef ROW_H
#define ROW_H

#include <stdint.h>

#include "storage/table_header.h"

typedef struct Table Table; 
typedef struct ASTNode ASTNode;
typedef struct WhereCondition WhereCondition;
typedef enum DataType DataType;

typedef enum RowResult {
    ROW_SUCCESS,
    ROW_INVALID_TABLE,
    ROW_NOT_FOUND,
    ROW_ALLOC_ERROR,
    ROW_VALUES_ALLOC_ERROR,
    ROW_ALLOC_PAGE_ERROR,
    ROW_GET_PAGE_ERROR,
    ROW_FLUSH_PAGE_ERROR,
} RowResult;

typedef struct Value {
    DataType type;
    union {
        uint32_t int_value;
        double real_value; 
        char *string_value;
        bool bool_value;
    };
} Value;

typedef struct Row {
    uint32_t id;
    uint32_t num_columns;
    uint32_t size;
    Value *values; // Array of values, one per column
} Row;

// row.c
RowResult insert_row(Table *table, ASTNode *node);
RowResult select_row(Table *table, ASTNode *node);
RowResult delete_row(Table *table, ASTNode *node);

// row_utils.c
bool evaluate_where_condition(Row *row, WhereCondition *where, Table *table);
RowResult initialize_row(Table *table, Row **row);
void free_row(Row *row);
void free_row_values(Row *row);
uint32_t calculate_row_size(Table *table);
uint32_t calculate_rows_per_page(Table *table);
uint32_t calculate_max_rows(Table *table);
void print_row(Row row, ASTNode *node, Table *table);

// row_storage.c
void serialize_row(Row *row, Table *table, void *destination); 
void deserialize_row(void *source, Row *row, Table *table);

#endif
