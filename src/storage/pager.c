#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/database.h"
#include "../../include/storage.h"
#include "hashmap.h"

Pager *pager_open(const char *filename) {
  int fd = open(filename,
                O_RDWR |     // Read/Write mode
                    O_CREAT, // Create file if it doesn't exist
                S_IWUSR |    // User write permission
                    S_IRUSR  // User read permission
  );

  if (fd == -1) {
    printf("Unable to open file: %s\n", strerror(errno));
    return NULL;
  }

  off_t file_length = lseek(fd, 0, SEEK_END);

  Pager *pager = malloc(sizeof(Pager));
  if (pager == NULL) {
    close(fd);
    return NULL;
  }

  pager->file_descriptor = fd;
  pager->file_length = file_length;
  pager->num_pages = (file_length - HEADER_SIZE) / PAGE_SIZE;

  // Initialize all page pointers to NULL
  memset(pager->pages, 0, sizeof(void *) * TABLE_MAX_PAGES);

  return pager;
}

void *pager_get_page(Pager *pager, uint32_t page_num) {
  if (page_num >= TABLE_MAX_PAGES) {
    printf("Page number out of bounds: %d > %d\n", page_num, TABLE_MAX_PAGES);
    return NULL;
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
        printf("Error reading file: %s\n", strerror(errno));
        return NULL;
      }
    }

    pager->pages[page_num] = page;
  }

  return pager->pages[page_num];
}

void pager_flush(Pager *pager, uint32_t page_num) {
  if (pager->pages[page_num] == NULL) {
    printf("Can't flush null page\n");
    return;
  }

  uint32_t header_offset = sizeof(TableHeader);
  off_t offset = lseek(pager->file_descriptor,
                       header_offset + (page_num * PAGE_SIZE), SEEK_SET);

  if (offset == -1) {
    printf("Error seeking in file: %s\n", strerror(errno));
    return;
  }

  ssize_t bytes_written =
      write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);

  if (bytes_written == -1) {
    printf("Error writing to file: %s\n", strerror(errno));
  }
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

void header_tables_restore(Pager *pager, HashMap *map) {
  TableHeader headers[MAX_TABLES];
  lseek(pager->file_descriptor, 0, SEEK_SET);
  read(pager->file_descriptor, headers, HEADER_SIZE);

  // Restore tables from headers
  for (int i = 0; i < MAX_TABLES; i++) {
    if (headers[i].table_name[0] != '\0') { // If table exists
      Table *table = malloc(sizeof(Table));
      table->pager = pager;
      table->num_rows = headers[i].num_rows;
      table->next_id = headers[i].next_id;
      strncpy(table->name, headers[i].table_name, MAX_NAME_LENGTH);
      hashmap_set(map, table->name, table);
    }
  }
}

void header_tables_store(Database *db) {
  // Save header for users table
  TableHeader header = {0}; // Initialize to 0
  Table *users_table = NULL;

  if (find_table(db, "users", &users_table) == TABLE_SUCCESS) {
    strncpy(header.table_name, "users", MAX_NAME_LENGTH);
    header.num_rows = users_table->num_rows;
    header.next_id = users_table->next_id;

    // Write header to start of file
    lseek(db->pager->file_descriptor, 0, SEEK_SET);
    write(db->pager->file_descriptor, &header, sizeof(TableHeader));

    // Close table and cleanup
    close_table(users_table, db->pager);
  }
}
