#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "core/table.h"

#include "core/row.h"
#include "storage/pager.h"
#include "storage/table_header.h"

void serialize_row(Row *row, Table *table, void *destination) {
  uint8_t *ptr = (uint8_t *)destination;

  uint32_t row_size = sizeof(row->row_size);
  for (uint32_t i = 0; i < table->num_columns; i++) {
    Value *value = &row->values[i];

    row_size += sizeof(value->type);
    switch (value->type) {
    case COLUMN_TYPE_INT:
      row_size += sizeof(value->int_value);
      break;
    case COLUMN_TYPE_TEXT:
      row_size += sizeof(uint32_t);
      row_size += strlen(value->string_value) + 1;
      break;
    default:
      fprintf(stderr, "Unknown columun type in serialize_row\n");
    }
  }

  // Write row_size field
  row->row_size = row_size;
  memcpy(ptr, &row->row_size, sizeof(row->row_size));
  ptr += sizeof(row->row_size);

  for (uint32_t i = 0; i < table->num_columns; i++) {
    Value *value = &row->values[i];

    memcpy(ptr, &value->type, sizeof(value->type));
    ptr += sizeof(value->type);

    switch (value->type) {
    case COLUMN_TYPE_INT:
      memcpy(ptr, &value->int_value, sizeof(value->int_value));
      ptr += sizeof(value->int_value);
      break;
    case COLUMN_TYPE_TEXT: {
      uint32_t str_len = strlen(value->string_value);
      memcpy(ptr, &str_len, sizeof(str_len));
      ptr += sizeof(str_len);
      memcpy(ptr, value->string_value, str_len + 1);
      ptr += str_len + 1;
      break;
    }
    default:
      fprintf(stderr, "Unknown columun type in serialize_row\n");
    }
  }
}

void deserialize_row(void *source, Row *row, Table *table) {
  uint8_t *ptr = (uint8_t *)source;

  memcpy(&row->row_size, ptr, sizeof(row->row_size));
  ptr += sizeof(row->row_size);

  row->values = malloc(table->num_columns * sizeof(Value));

  for (uint32_t i = 0; i < table->num_columns; i++) {
    Value *value = &row->values[i];

    memcpy(&value->type, ptr, sizeof(value->type));
    ptr += sizeof(value->type);

    switch (value->type) {
    case COLUMN_TYPE_INT:
      memcpy(&value->int_value, ptr, sizeof(value->int_value));
      ptr += sizeof(value->int_value);
      break;
    case COLUMN_TYPE_TEXT: {
      uint32_t str_len;
      memcpy(&str_len, ptr, sizeof(str_len));
      ptr += sizeof(str_len);

      value->string_value = malloc(str_len + 1);
      memcpy(value->string_value, ptr, str_len + 1);
      ptr += str_len + 1;
      break;
    }
    default:
      fprintf(stderr, "Unknown columun type in deserialize_row\n");
    }
  }
}
