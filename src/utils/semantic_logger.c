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
  case SEMANTIC_DUPLICATE_TABLE:
    fprintf(stderr, "Semantic Error: Table '%s' already exists\n",
            error->found ? error->found : "unknown");
    break;
  case SEMANTIC_DUPLICATE_COLUMN:
    fprintf(stderr, "Semantic Error: Column '%s' is already defined\n",
            error->found ? error->found : "unknown");
    break;
  case SEMANTIC_COLUMN_COUNT_MISMATCH:
    fprintf(
        stderr,
        "Semantic Error: Table has %s columns but %s values were supplied\n",
        error->found, error->expected);
    break;
  case SEMANTIC_INVALID_NAME_LENGTH:
    fprintf(stderr,
            "Semantic Error: '%s' exceeds maximum length of 64 characters\n",
            error->found);
    break;
  default:
    fprintf(stderr, "Semantic Error: Unknown semantic error\n");
    break;
  }
}
