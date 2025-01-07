
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../../include/database.h"
#include "../../include/storage.h"

void *get_row_slot(Table *table, uint32_t row_num) {
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = table->pages[page_num];
  if (page == NULL) {
    page = table->pages[page_num] = malloc(PAGE_SIZE);
  }

  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}
