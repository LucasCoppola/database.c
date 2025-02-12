#ifndef ROW_H
#define ROW_H

#include <stdint.h>

#include "storage/table_header.h"

typedef struct Table Table; 

typedef enum RowResult {
    ROW_SUCCESS,
    ROW_NAME_TOO_LONG,
    ROW_INVALID_TABLE,
    ROW_NOT_FOUND,
    ROW_ALLOC_PAGE_ERROR,
    ROW_GET_PAGE_ERROR,
    ROW_FLUSH_PAGE_ERROR,
} RowResult;

typedef struct Row {
    uint32_t id;
    char username[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
} Row;

// row_storage.c
void serialize_row(Row *source, void *destination); 
void deserialize_row(void *source, Row *destination);
void *get_row_slot(Table *table, uint32_t row_num);

#endif
