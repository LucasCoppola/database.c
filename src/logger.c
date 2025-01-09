#include <stdio.h>
#include <string.h>

#include "../include/database.h"
#include "../include/hashmap.h"
#include "../include/logger.h"

void log_error(const ErrorInfo *error) {
  fprintf(stderr, "Error in %s: %s.\n", error->context, error->message);
}

const char *get_error_message(const char *context, int code) {
  if (context == NULL) {
    return "Unknown context";
  }

  if (strcmp(context, "hashmap") == 0) {
    return hashmap_error_string(code);
  }

  if (strcmp(context, "table") == 0) {
    return table_error_string(code);
  }

  if (strcmp(context, "database") == 0) {
    return database_error_string(code);
  }

  if (strcmp(context, "row") == 0) {
    return row_error_string(code);
  }

  return "Unknown error context";
}

const char *database_error_string(DatabaseResult result) {
  switch (result) {
  case DATABASE_ALLOC_ERROR:
    return "Failed to allocate memory for database";
  case DATABASE_HASHMAP_INIT_ERROR:
    return "Failed to initialize hashmap";
  default:
    return "Unrecognized database error";
  }
}

const char *hashmap_error_string(HashMapResult result) {
  switch (result) {
  case HASHMAP_ALLOCATION_FAILURE:
    return "Failed to allocate memory for hashmap";
  case BUCKETS_ALLOCATION_FAILURE:
    return "Failed to allocate memory for hashmap buckets";
  case HASHMAP_RESIZE_FAILURE:
    return "Failed to resize hashmap";
  case HASHMAP_KEY_NOT_FOUND:
    return "Key not found in hashmap";
  case HASHMAP_INVALID_MAP:
    return "Invalid hashmap pointer";
  case HASHMAP_UNKNOWN_ERROR:
    return "Unknown hashmap error";
  default:
    return "Unrecognized hashmap error";
  }
}

const char *table_error_string(TableResult result) {
  switch (result) {
  case TABLE_INVALID_DB:
    return "Invalid database";
  case TABLE_ALLOC_ERROR:
    return "Failed to allocate memory for table";
  case TABLE_NAME_TOO_LONG:
    return "Table name is too long";
  case TABLE_HASHMAP_SET_ERROR:
    return "Failed to set table in hashmap";
  case TABLE_NOT_FOUND:
    return "Table not found";
  default:
    return "Unrecognized table error";
  }
}

const char *row_error_string(RowResult result) {
  switch (result) {
  case ROW_INVALID_TABLE:
    return "Invalid table";
  case ROW_NAME_TOO_LONG:
    return "Name is too long";
  case ROW_NOT_FOUND:
    return "Row not found";
  default:
    return "Unrecognized row error";
  }
}
