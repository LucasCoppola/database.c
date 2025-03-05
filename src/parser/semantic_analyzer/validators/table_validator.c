#include <stdlib.h>

#include "core/table.h"
#include "parser/semantic_analyzer.h"

Table *semantic_validate_table_exists(Database *db, char *table_name) {
  Table *table = NULL;
  TableResult table_result = table_find(db, table_name, &table);
  if (table_result != TABLE_SUCCESS) {
    return NULL;
  }

  return table;
}
