#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  Table* table;
  uint32_t row_num;
  bool end_of_table;
} Cursor;

// cursor.c
Cursor *table_start(Table *table);
Cursor *table_end(Table *table);
void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor); 

#endif
