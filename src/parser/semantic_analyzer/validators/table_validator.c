#include <stdlib.h>

#include "core/table.h"
#include "parser/semantic_analyzer.h"

SemanticResult semantic_validate_table_existance(Database *db,
                                                 char *table_name) {
  Table *table = NULL;
  TableResult table_result = table_find(db, table_name, &table);
  if (table_result != TABLE_SUCCESS) {
    return SEMANTIC_TABLE_NOT_FOUND;
  }

  return SEMANTIC_SUCCESS;
}

SemanticResult semantic_validate_no_duplicate_tables(Database *db,
                                                     char *table_name) {
  Table *table = NULL;
  TableResult table_result = table_find(db, table_name, &table);
  if (table_result == TABLE_SUCCESS) {
    return SEMANTIC_DUPLICATE_TABLE;
  }

  return SEMANTIC_SUCCESS;
}
