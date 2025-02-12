#ifndef PAGER_H
#define PAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define PAGE_SIZE 4096

typedef struct Table Table;

typedef enum PagerResult {
   PAGER_SUCCESS,
   PAGER_OPEN_ERROR,
   PAGER_READ_ERROR,
   PAGER_WRITE_ERROR,
   PAGER_ALLOC_ERROR,
   PAGER_INVALID_ARGS,
   PAGER_INVALID_PAGE,
} PagerResult;

typedef struct Pager {
  int file_descriptor;
  uint32_t file_length;
  uint32_t num_pages;
  uint32_t num_tables;
} Pager;

// pager.c
PagerResult pager_init(const char *filename, Pager **out_pager);
PagerResult pager_page_load(Pager *pager, uint32_t page_num, Table *table, void **out_page);
PagerResult pager_page_flush(Pager *pager, uint32_t page_num, Table *table);
PagerResult pager_pages_flush(Pager *pager, Table *table);
PagerResult pager_page_alloc(uint32_t page_num, Table *table);
void pager_close(Pager *pager);

// pager_utils.c
PagerResult pager_header_read(Pager *pager);
bool pager_write_at_offset(Pager *pager, off_t offset, const void *buf, size_t size);
bool pager_read_at_offset(Pager *pager, off_t offset, void *buf, size_t size);

#endif
