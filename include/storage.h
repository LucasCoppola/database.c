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
  PAGER_INVALID_PAGE,
} PagerResult;

typedef struct {
    uint32_t next_id;
    uint32_t num_rows;
    uint32_t page_offset;
    char table_name[MAX_NAME_LENGTH];
} TableHeader;

#define HEADER_SIZE (sizeof(TableHeader))

// row_storage.c
void serialize_row(Row *source, void *destination); 
void deserialize_row(void *source, Row *destination);
void *get_row_slot(Table *table, uint32_t row_num);

// pager.c
PagerResult pager_open(const char *filename, Pager **out_pager);
PagerResult pager_get_page(Pager *pager, uint32_t page_num, Table *table, void **out_page);
PagerResult pager_flush(Pager *pager, uint32_t page_num, Table *table);
void pager_load_page(Pager *pager, uint32_t page_num, Table *table);
void pager_flush_all(Pager *pager, Table *table);
void pager_close(Pager *pager);
void pager_alloc_page(uint32_t page_num, Table *table);

// header_restore.c
void header_tables_restore(Pager *pager, HashMap *map);
void initialize_table_from_header(TableHeader *header, Pager *pager, HashMap *map);
ssize_t read_table_header(int file_descriptor, off_t *header_pos, TableHeader *header);
uint32_t read_num_tables(int file_descriptor);
void header_pages_restore(uint32_t num_rows, Pager *pager);

// header_store.c
void header_tables_store(Database *db);
void header_tables_write(Database *db);
void header_table_write(Pager *pager, Table *table, size_t idx);
void header_tables_count_write(Pager *pager, uint32_t num_tables);

// cursor.c
Cursor *table_start(Table *table);
Cursor *table_end(Table *table);
void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor); 

extern const uint32_t TABLE_MAX_ROWS;  
extern const uint32_t ROWS_PER_PAGE;  
extern const uint32_t ROW_SIZE;  

#endif
