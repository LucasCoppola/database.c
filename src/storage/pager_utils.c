#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "core/database.h"
#include "storage/pager.h"
#include "storage/table_header.h"

PagerResult pager_header_read(Pager *pager) {
  if ((size_t)pager->file_length < sizeof(uint32_t)) {
    return PAGER_SUCCESS; // Empty file
  }

  if (!pager_read_at_offset(pager, 0, &pager->num_tables, sizeof(uint32_t))) {
    return PAGER_READ_ERROR;
  }

  off_t expected_size = sizeof(uint32_t) + (pager->num_tables * HEADER_SIZE);
  if (pager->file_length < expected_size) {
    return PAGER_READ_ERROR;
  }

  for (uint32_t i = 0; i < pager->num_tables; i++) {
    TableHeader header;
    if (!pager_read_at_offset(pager, sizeof(uint32_t) + i * HEADER_SIZE,
                              &header, HEADER_SIZE)) {
      return PAGER_READ_ERROR;
    }
  }

  return PAGER_SUCCESS;
}

bool pager_read_at_offset(Pager *pager, off_t offset, void *buf, size_t size) {
  if (lseek(pager->file_descriptor, offset, SEEK_SET) == -1)
    return false;
  return (size_t)read(pager->file_descriptor, buf, size) == size;
}

bool pager_write_at_offset(Pager *pager, off_t offset, const void *buf,
                           size_t size) {
  if (lseek(pager->file_descriptor, offset, SEEK_SET) == -1)
    return false;
  return (size_t)write(pager->file_descriptor, buf, size) == size;
}
