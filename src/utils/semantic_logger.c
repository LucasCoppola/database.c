#include <stdio.h>

#include "parser/semantic_analyzer.h"
#include "utils/semantic_logger.h"

void semantic_error_report(const SemanticError *error) {
  switch (error->code) {
  case SEMANTIC_TABLE_NOT_FOUND:
    fprintf(stderr, "Semantic Error: Unable to find table '%s'\n",
            error->found ? error->found : "unknown");
    break;
  case SEMANTIC_COLUMN_NOT_FOUND:
    fprintf(stderr, "Semantic Error: Unable to find column '%s'\n",
            error->found ? error->found : "unknown");
    break;
  case SEMANTIC_TYPE_MISMATCH:
    fprintf(stderr,
            "Semantic Error: Type mismatch - found '%s', expected '%s'\n",
            error->found ? error->found : "unknown",
            error->expected ? error->expected : "unknown");
    break;
  case SEMANTIC_DUPLICATE_TABLE:
    fprintf(stderr,
            "Semantic Error: Cannot create table - '%s' already exists\n",
            error->found ? error->found : "unknown");
    break;
  case SEMANTIC_DUPLICATE_COLUMN:
    fprintf(stderr, "Semantic Error: Column '%s' is already defined\n",
            error->found ? error->found : "unknown");
    break;
  default:
    fprintf(stderr, "Semantic Error: Unknown semantic error\n");
    break;
  }
}
