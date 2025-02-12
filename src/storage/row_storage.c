#include <stdint.h>
#include <string.h>
#include <strings.h>

#include "core/row.h"
#include "core/table.h"
#include "storage/pager.h"
#include "storage/table_header.h"

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void serialize_row(Row *source, void *destination) {
  if (source == NULL || destination == NULL) {
    return; // Handle null pointers
  }

  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  memcpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
  memcpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination) {
  if (source == NULL || destination == NULL) {
    return; // Handle null pointers
  }

  memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy(destination->username, source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(destination->email, source + EMAIL_OFFSET, EMAIL_SIZE);

  // Ensure null termination for strings
  destination->username[USERNAME_SIZE - 1] = '\0'; // Null-terminate username
  destination->email[EMAIL_SIZE - 1] = '\0';       // Null-terminate email
}
