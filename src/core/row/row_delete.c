#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "core/table.h"

#include "core/hashmap.h"
#include "core/row.h"
#include "parser/ast.h"
#include "storage/pager.h"
#include "utils/logger.h"

RowResult delete_row(Table *table, ASTNode *node) {
  if (table == NULL) {
    return ROW_INVALID_TABLE;
  }

  uint32_t row_size = calculate_row_size(table);
  uint32_t rows_per_page = calculate_rows_per_page(table);
  bool deleted_any = false;
  uint32_t index = 0;

  while (index < table->num_rows) {
    uint32_t page_num = index / rows_per_page;
    uint32_t offset = index % rows_per_page;

    void *page;
    PagerResult pager_result =
        pager_page_load(table->pager, page_num, table, &page);
    if (pager_result != PAGER_SUCCESS) {
      return ROW_GET_PAGE_ERROR;
    }

    // Get the row's serialized location.
    void *row_location = (char *)page + offset * row_size;

    // Deserialize the row for evaluation.
    Row temp;
    temp.values = malloc(table->num_columns * sizeof(Value));
    if (!temp.values) {
      return ROW_VALUES_ALLOC_ERROR;
    }
    deserialize_row(row_location, &temp, table);

    bool should_delete = false;
    if (has_where_condition(node)) {
      should_delete =
          evaluate_where_condition(&temp, &node->where_condition, table);
    } else {
      should_delete = true;
    }
    free(temp.values);

    if (should_delete) {
      uint32_t last_index = table->num_rows - 1;
      // If the current row is not the last row, swap it with the last row.
      if (index != last_index) {
        uint32_t last_page_num = last_index / rows_per_page;
        uint32_t last_offset = last_index % rows_per_page;
        void *last_page;
        pager_result =
            pager_page_load(table->pager, last_page_num, table, &last_page);
        if (pager_result != PAGER_SUCCESS) {
          return ROW_GET_PAGE_ERROR;
        }
        void *last_row_location = (char *)last_page + last_offset * row_size;
        memcpy(row_location, last_row_location, row_size);

        // Flush the page from which we read the last row.
        PagerResult flush_res =
            pager_page_flush(table->pager, last_page_num, table);
        if (flush_res != PAGER_SUCCESS) {
          return ROW_FLUSH_PAGE_ERROR;
        }
      }
      table->num_rows--;
      deleted_any = true;
      continue;
    }
    index++;
  }

  uint32_t flush_page = table->num_rows / rows_per_page;
  PagerResult flush_res = pager_page_flush(table->pager, flush_page, table);
  if (flush_res != PAGER_SUCCESS) {
    return ROW_FLUSH_PAGE_ERROR;
  }

  return deleted_any ? ROW_SUCCESS : ROW_NOT_FOUND;
}
