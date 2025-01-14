#ifndef STORAGE_H
#define STORAGE_H

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

#include <stdint.h>
#include "database.h"

typedef enum {
  PAGER_SUCCESS,
  PAGER_OPEN_ERROR,
  PAGER_ALLOC_ERROR,
  PAGER_OUT_OF_BOUNDS,
  PAGER_READ_ERROR,
  PAGER_WRITE_ERROR,
  PAGER_SEEK_ERROR,
  PAGER_INVALID_PAGE,
} PagerResult;

typedef struct {
    char table_name[MAX_NAME_LENGTH];
    uint32_t num_rows;
    uint32_t next_id;
} TableHeader;

#define HEADER_SIZE (sizeof(TableHeader) * MAX_TABLES)

void serialize_row(Row *source, void *destination); 
void deserialize_row(void *source, Row *destination);
void *get_row_slot(Table *table, uint32_t row_num);

PagerResult pager_open(const char *filename, Pager **out_pager);
PagerResult pager_get_page(Pager *pager, uint32_t page_num, void **out_page);
PagerResult pager_flush(Pager *pager, uint32_t page_num);
void pager_close(Pager *pager);

void header_tables_restore(Pager *pager, HashMap *map);
void header_tables_store(Database *db);

Cursor *table_start(Table *table);
Cursor *table_end(Table *table);
void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor); 

extern const uint32_t TABLE_MAX_ROWS;  
extern const uint32_t ROWS_PER_PAGE;  
extern const uint32_t ROW_SIZE ;  

#endif
