#include <stdlib.h>

#include "storage/pager.h"

#include "core/table.h"
#include "storage/cursor.h"
#include "utils/logger.h"

static void cursor_init(Cursor *cursor, Table *table, uint32_t row_num,
                        bool end_of_table) {
  cursor->table = table;
  cursor->row_num = row_num;
  cursor->end_of_table = end_of_table;
}

Cursor *table_start(Table *table) {
  Cursor *cursor = malloc(sizeof(Cursor));
  if (cursor == NULL) {
    return NULL;
  }
  cursor_init(cursor, table, 0, (table->num_rows == 0));
  return cursor;
}

Cursor *table_end(Table *table) {
  Cursor *cursor = malloc(sizeof(Cursor));
  if (cursor == NULL) {
    return NULL;
  }
  cursor_init(cursor, table, table->num_rows, true);
  return cursor;
}

void *cursor_value(Cursor *cursor) {
  uint32_t row_num = cursor->row_num;
  uint32_t page_num = row_num / ROWS_PER_PAGE;

  void *page = NULL;
  PagerResult result =
      pager_page_load(cursor->table->pager, page_num, cursor->table, &page);
  if (result != PAGER_SUCCESS || page == NULL) {
    LOG_ERROR("pager", result);
    return NULL;
  }

  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

void cursor_advance(Cursor *cursor) {
  cursor->row_num += 1;
  if (cursor->row_num >= cursor->table->num_rows) {
    cursor->end_of_table = true;
  }
}
