#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/storage.h"

PagerResult pager_open(const char *filename, Pager **out_pager) {
  int fd = open(filename,
                O_RDWR |     // Read/Write mode
                    O_CREAT, // Create file if it doesn't exist
                S_IWUSR |    // User write permission
                    S_IRUSR  // User read permission
  );

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

PagerResult pager_get_page(Pager *pager, uint32_t page_num, void **out_page) {
  if (page_num >= TABLE_MAX_PAGES) {
    return PAGER_OUT_OF_BOUNDS;
  }

  if (pager->pages[page_num] == NULL) {
    void *page = malloc(PAGE_SIZE);

    uint32_t header_offset = sizeof(TableHeader); // Account for header
    if (page_num <= pager->num_pages) {
      lseek(pager->file_descriptor, header_offset + (page_num * PAGE_SIZE),
            SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if (bytes_read == -1) {
        free(page);
        printf("PAGER_READ_ERROR: %s\n", strerror(errno));
        return PAGER_READ_ERROR;
      }
    }

    pager->pages[page_num] = page;
  }

  *out_page = pager->pages[page_num];
  return PAGER_SUCCESS;
}

PagerResult pager_flush(Pager *pager, uint32_t page_num) {
  if (pager->pages[page_num] == NULL) {
    return PAGER_INVALID_PAGE;
  }

  uint32_t header_offset = sizeof(TableHeader);
  off_t offset = lseek(pager->file_descriptor,
                       header_offset + (page_num * PAGE_SIZE), SEEK_SET);

  if (offset == -1) {
    printf("PAGER_SEEK_ERROR: %s\n", strerror(errno));
    return PAGER_SEEK_ERROR;
  }

  ssize_t bytes_written =
      write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);

  if (bytes_written == -1) {
    printf("PAGER_WRITE_ERROR: %s\n", strerror(errno));
    return PAGER_WRITE_ERROR;
  }

  return PAGER_SUCCESS;
}

void pager_close(Pager *pager) {
  // Flush all pages that have data
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    if (pager->pages[i] != NULL) {
      pager_flush(pager, i);
      free(pager->pages[i]);
      pager->pages[i] = NULL;
    }
  }

  close(pager->file_descriptor);
  free(pager);
}
