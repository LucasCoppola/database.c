#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
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
  pager->num_tables = 0;

  // Read number of tables if file is not empty
  if ((size_t)file_length >= sizeof(uint32_t)) {
    lseek(fd, 0, SEEK_SET);
    ssize_t bytes_read = read(fd, &pager->num_tables, sizeof(uint32_t));
    if (bytes_read != sizeof(uint32_t)) {
      free(pager);
      close(fd);
      return PAGER_ALLOC_ERROR;
    }

    // Check if file has enough space for all table headers
    off_t expected_size = sizeof(uint32_t) + (pager->num_tables * HEADER_SIZE);
    if (file_length < expected_size) {
      printf(
          "File appears to be corrupted: expected size %ld, actual size %ld\n",
          expected_size, file_length);
      free(pager);
      close(fd);
      return PAGER_ALLOC_ERROR;
    }
    printf("Read num_tables: %d\n", pager->num_tables);

    // Read and print table headers
    for (uint32_t i = 0; i < pager->num_tables; i++) {
      TableHeader header;
      ssize_t header_read = read(fd, &header, HEADER_SIZE);
      if (header_read != HEADER_SIZE) {
        printf("Failed to read header %d: %s\n", i, strerror(errno));
        free(pager);
        close(fd);
        return PAGER_ALLOC_ERROR;
      }
      printf("Read table header %d: name=%s, rows=%d, offset=%u\n", i,
             header.table_name, header.num_rows, header.page_offset);
    }
  }

  uint32_t header_offset = sizeof(uint32_t) + (HEADER_SIZE * pager->num_tables);
  pager->num_pages = (file_length > header_offset)
                         ? (file_length - header_offset) / PAGE_SIZE
                         : 0;

  *out_pager = pager;
  return PAGER_SUCCESS;
}

// loads a page from disk into memory
void pager_load_page(Pager *pager, uint32_t page_num, Table *table) {
  void *page = malloc(PAGE_SIZE);
  if (page == NULL) {
    printf("Failed to allocate memory for page %u\n", page_num);
    return;
  }

  uint32_t offset = table->page_offset + (page_num * PAGE_SIZE);
  memset(page, 0, PAGE_SIZE); // Initialize page to zeros

  if (offset < pager->file_length) { // Check if this offset exists in file
    lseek(pager->file_descriptor, offset, SEEK_SET);
    ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
    if (bytes_read == -1) {
      printf("Error reading page %u: %s\n", page_num, strerror(errno));
      free(page);
      return;
    }
    printf("Read %zd bytes for page %u at offset %u\n", bytes_read, page_num,
           offset);
  }

  table->pages[page_num] = page;
}

PagerResult pager_get_page(Pager *pager, uint32_t page_num, Table *table,
                           void **out_page) {
  if (page_num >= TABLE_MAX_PAGES) {
    return PAGER_OUT_OF_BOUNDS;
  }

  if (table->pages[page_num] == NULL) {
    pager_load_page(pager, page_num, table);
  }

  *out_page = table->pages[page_num];
  return PAGER_SUCCESS;
}

void pager_alloc_page(uint32_t page_num, Table *table) {
  void *page = table->pages[page_num];
  if (page != NULL) {
    printf("Page already allocated\n");
    return;
  }

  page = malloc(PAGE_SIZE);
  if (page == NULL) {
    printf("Memory allocation failed for page\n");
    return;
  }
  memset(page, 0, PAGE_SIZE); // Initialize page to zeros
  table->pages[page_num] = page;
}

PagerResult pager_flush(Pager *pager, uint32_t page_num, Table *table) {
  if (table->pages[page_num] == NULL) {
    return PAGER_INVALID_PAGE;
  }

  uint32_t offset = table->page_offset + (page_num * PAGE_SIZE);
  printf("Flushing page %u (index %u) for table %s at offset %u (table offset: "
         "%u)\n",
         page_num, page_num, table->name, offset, table->page_offset);

  lseek(pager->file_descriptor, offset, SEEK_SET);
  ssize_t bytes_written =
      write(pager->file_descriptor, table->pages[page_num], PAGE_SIZE);

  if (bytes_written == -1) {
    printf("Error writing page: %s\n", strerror(errno));
    return PAGER_INVALID_PAGE;
  }
  if (bytes_written != PAGE_SIZE) {
    printf("Warning: Only wrote %zd bytes instead of %d\n", bytes_written,
           PAGE_SIZE);
    return PAGER_INVALID_PAGE;
  }

  if (offset + PAGE_SIZE > pager->file_length) {
    pager->file_length = offset + PAGE_SIZE;
  }

  return PAGER_SUCCESS;
}

void pager_flush_all(Pager *pager, Table *table) {
  if (pager == NULL) {
    return;
  }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    if (table->pages[i] != NULL) {
      pager_flush(pager, i, table);
      free(table->pages[i]);
      table->pages[i] = NULL;
    }
  }
}

void pager_close(Pager *pager) {
  close(pager->file_descriptor);
  free(pager);
}
