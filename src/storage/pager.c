#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/storage.h"

PagerResult pager_init(const char *filename, Pager **out_pager) {
  int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
  if (fd == -1)
    return PAGER_OPEN_ERROR;

  Pager *pager = malloc(sizeof(Pager));
  if (!pager) {
    close(fd);
    return PAGER_ALLOC_ERROR;
  }

  pager->file_descriptor = fd;
  pager->file_length = lseek(fd, 0, SEEK_END);
  pager->num_tables = 0;

  if (pager_header_read(pager) != PAGER_SUCCESS) {
    pager_close(pager);
    return PAGER_READ_ERROR;
  }

  // Calculates number of data pages
  uint32_t header_offset = sizeof(uint32_t) + (HEADER_SIZE * pager->num_tables);
  pager->num_pages = (pager->file_length > header_offset)
                         ? (pager->file_length - header_offset) / PAGE_SIZE
                         : 0;

  *out_pager = pager;
  return PAGER_SUCCESS;
}

PagerResult pager_page_load(Pager *pager, uint32_t page_num, Table *table,
                            void **out_page) {
  if (!pager || !table || page_num >= TABLE_MAX_PAGES)
    return PAGER_INVALID_ARGS;

  if (table->pages[page_num]) {
    *out_page = table->pages[page_num];
    return PAGER_SUCCESS;
  }

  void *page = calloc(1, PAGE_SIZE);
  if (!page)
    return PAGER_ALLOC_ERROR;

  uint32_t offset = table->page_offset + (page_num * PAGE_SIZE);
  if (offset < pager->file_length) {
    if (!pager_read_at_offset(pager, offset, page, PAGE_SIZE)) {
      free(page);
      return PAGER_READ_ERROR;
    }
  }

  table->pages[page_num] = page;
  *out_page = page;
  return PAGER_SUCCESS;
}

PagerResult pager_page_flush(Pager *pager, uint32_t page_num, Table *table) {
  if (!pager || !table || !table->pages[page_num])
    return PAGER_INVALID_ARGS;

  uint32_t offset = table->page_offset + (page_num * PAGE_SIZE);
  if (!pager_write_at_offset(pager, offset, table->pages[page_num],
                             PAGE_SIZE)) {
    return PAGER_WRITE_ERROR;
  }

  if (offset + PAGE_SIZE > pager->file_length) {
    pager->file_length = offset + PAGE_SIZE;
  }
  return PAGER_SUCCESS;
}

PagerResult pager_pages_flush(Pager *pager, Table *table) {
  if (!pager || !table)
    return PAGER_INVALID_ARGS;

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    if (table->pages[i]) {
      PagerResult result = pager_page_flush(pager, i, table);
      if (result != PAGER_SUCCESS) {
        return result;
      }
      free(table->pages[i]);
      table->pages[i] = NULL;
    }
  }
  return PAGER_SUCCESS;
}

PagerResult pager_page_alloc(uint32_t page_num, Table *table) {
  if (!table || page_num >= TABLE_MAX_PAGES) {
    return PAGER_INVALID_ARGS;
  }
  if (table->pages[page_num]) {
    return PAGER_SUCCESS;
  }

  void *page = calloc(1, PAGE_SIZE);
  if (!page)
    return PAGER_ALLOC_ERROR;

  table->pages[page_num] = page;
  return PAGER_SUCCESS;
}

void pager_close(Pager *pager) {
  if (!pager)
    return;
  close(pager->file_descriptor);
  free(pager);
}
