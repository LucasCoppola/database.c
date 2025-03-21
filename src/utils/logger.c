#include <stdio.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"

#include "core/hashmap.h"
#include "core/row.h"
#include "parser/ast.h"
#include "parser/tokenizer.h"
#include "storage/pager.h"
#include "utils/logger.h"

static struct {
  Error current;
  char formatted[MAX_ERROR_LENGTH];
  bool has_error;
} ErrorState = {0};

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

  if (strcmp(context, "pager") == 0) {
    return pager_error_string(code);
  }

  if (strcmp(context, "ast") == 0) {
    return ast_error_string(code);
  }

  if (strcmp(context, "tokenizer") == 0) {
    return tokenizer_error_string(code);
  }

  return "Unknown error context";
}

void error_report(const Error *error) {
  ErrorState.current = *error;
  ErrorState.has_error = true;

  const char *error_msg = get_error_message(error->component, error->code);

  snprintf(ErrorState.formatted, MAX_ERROR_LENGTH,
           "[%s:%d] %s::%s failed: %s (code %d)", error->file, error->line,
           error->component, error->operation, error_msg, error->code);

  fprintf(stderr, "%s\n", ErrorState.formatted);
}

const char *database_error_string(DatabaseResult result) {
  switch (result) {
  case DATABASE_ALLOC_ERROR:
    return "Failed to allocate memory for database";
  case DATABASE_HASHMAP_INIT_ERROR:
    return "Failed to initialize hashmap";
  case DATABASE_PAGER_INIT_ERROR:
    return "Failed to initialize pager";
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
  case TABLE_CREATE_ERROR:
    return "Failed to set table in hashmap";
  case TABLE_DELETE_ERROR:
    return "Failed to delete table in hashmap";
  default:
    return "Unrecognized table error";
  }
}

const char *row_error_string(RowResult result) {
  switch (result) {
  case ROW_INVALID_TABLE:
    return "Invalid table";
  case ROW_NOT_FOUND:
    return "Row not found";
  case ROW_ALLOC_PAGE_ERROR:
    return "Failed to allocate page";
  case ROW_GET_PAGE_ERROR:
    return "Failed to get page";
  case ROW_FLUSH_PAGE_ERROR:
    return "Failed to flush page";
  default:
    return "Unrecognized row error";
  }
}

const char *pager_error_string(PagerResult result) {
  switch (result) {
  case PAGER_OPEN_ERROR:
    return "Failed to open file";
  case PAGER_READ_ERROR:
    return "Failed to read from file";
  case PAGER_WRITE_ERROR:
    return "Failed to write to file";
  case PAGER_ALLOC_ERROR:
    return "Failed to allocate memory for pager";
  case PAGER_INVALID_ARGS:
    return "Invalid arguments";
  case PAGER_INVALID_PAGE:
    return "Invalid page";
  default:
    return "Unrecognized pager error";
  }
}

const char *ast_error_string(ASTNodeResult result) {
  switch (result) {
  case AST_ALLOC_ERROR:
    return "Failed to allocate memory for AST node";
  default:
    return "Unrecognized AST node error";
  }
}

const char *tokenizer_error_string(TokenizerResult result) {
  switch (result) {
  case TOKENIZER_STATE_ALLOC_ERROR:
    return "Failed to allocate memory for tokenizer state";
  case TOKENIZER_QUERY_ALLOC_ERROR:
    return "Failed to allocate memory for query string";
  case TOKENIZER_TOKENS_ALLOC_ERROR:
    return "Failed to allocate memory for tokens";
  case TOKENIZER_READ_WORD_ERROR:
    return "Failed to read word";
  case TOKENIZER_READ_NUMBER_ERROR:
    return "Failed to read number";
  case TOKENIZER_READ_STRING_ERROR:
    return "Failed to read string";
  default:
    return "Unrecognized tokenizer error";
  }
}
