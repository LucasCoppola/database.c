#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/storage.h"

PagerResult pager_open(const char *filename, Pager **out_pager) {
  int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
  if (fd == -1) {
    printf("PAGER_OPEN_ERROR: %s\n", strerror(errno));
    return PAGER_OPEN_ERROR;
  }

  off_t file_length = lseek(fd, 0, SEEK_END);

  Pager *pager = malloc(sizeof(Pager));
  if (pager == NULL) {
    close(fd);
    return PAGER_ALLOC_ERROR;
  }

  pager->file_descriptor = fd;
  pager->file_length = file_length;
  pager->num_pages = (file_length - HEADER_SIZE) / PAGE_SIZE;

  // Initialize all page pointers to NULL
  memset(pager->pages, 0, sizeof(void *) * TABLE_MAX_PAGES);

  *out_pager = pager;
  return PAGER_SUCCESS;
}

// loads a page from disk into memory
void pager_load_page(Pager *pager, uint32_t page_num) {
  void *page = malloc(PAGE_SIZE);
  uint32_t offset = sizeof(TableHeader) + (page_num * PAGE_SIZE);

  if (page_num <= pager->num_pages) {
    lseek(pager->file_descriptor, offset, SEEK_SET);
    ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
    if (bytes_read == -1) {
      free(page);
      return;
    }
  }

  pager->pages[page_num] = page;
}

PagerResult pager_get_page(Pager *pager, uint32_t page_num, void **out_page) {
  if (page_num >= TABLE_MAX_PAGES) {
    return PAGER_OUT_OF_BOUNDS;
  }

  if (pager->pages[page_num] == NULL) {
    pager_load_page(pager, page_num);
  }

  *out_page = pager->pages[page_num];
  return PAGER_SUCCESS;
}

void pager_alloc_page(Pager *pager, uint32_t page_num) {
  void *page = pager->pages[page_num];
  if (page != NULL) {
    return;
  }

  page = malloc(PAGE_SIZE);
  if (page == NULL) {
    printf("Memory allocation failed for page\n");
    return;
  }

  pager->pages[page_num] = page;
}

PagerResult pager_flush(Pager *pager, uint32_t page_num) {
  if (pager->pages[page_num] == NULL) {
    return PAGER_INVALID_PAGE;
  }

  uint32_t offset = sizeof(TableHeader) + (page_num * PAGE_SIZE);
  lseek(pager->file_descriptor, offset, SEEK_SET);
  write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);

  return PAGER_SUCCESS;
}

void pager_flush_all(Pager *pager) {
  if (pager == NULL) {
    return;
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    if (pager->pages[i] != NULL) {
      pager_flush(pager, i);
      free(pager->pages[i]);
      pager->pages[i] = NULL;
    }
  }
}

void pager_close(Pager *pager) {
  close(pager->file_descriptor);
  free(pager);
}
