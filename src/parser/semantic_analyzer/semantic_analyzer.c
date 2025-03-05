#include <inttypes.h>

#include "core/database.h"
#include "core/table.h"

#include "parser/ast.h"
#include "parser/semantic_analyzer.h"
#include "utils/semantic_logger.h"

SemanticResult semantic_analyze(Database *db, ASTNode *node) {
  switch (node->type) {
  case NODE_CREATE_TABLE:
    return semantic_analyze_create_table(db, node);
  case NODE_DROP_TABLE:
    return semantic_analyze_drop_table(db, node);
  case NODE_INSERT:
    return semantic_analyze_insert(db, node);
  case NODE_SELECT:
    // return semantic_analyze_select(db, node);
    break;
  default:
    return SEMANTIC_SUCCESS;
  }
  return SEMANTIC_SUCCESS;
}

SemanticResult semantic_analyze_create_table(Database *db, ASTNode *node) {
  Table *table = semantic_validate_table_exists(db, node->table_name);
  if (table) {
    SEMANTIC_LOG_ERROR(SEMANTIC_DUPLICATE_TABLE, node->table_name, NULL);
    return SEMANTIC_DUPLICATE_TABLE;
  }

  char *out_column = NULL;
  bool is_column_unique = semantic_validate_columns_unique(
      node->create_table.columns, node->create_table.num_columns, &out_column);
  if (!is_column_unique) {
    SEMANTIC_LOG_ERROR(SEMANTIC_DUPLICATE_COLUMN, out_column, NULL);
    return SEMANTIC_DUPLICATE_COLUMN;
  }

  return SEMANTIC_SUCCESS;
}

SemanticResult semantic_analyze_drop_table(Database *db, ASTNode *node) {
  Table *table = semantic_validate_table_exists(db, node->table_name);
  if (!table) {
    SEMANTIC_LOG_ERROR(SEMANTIC_TABLE_NOT_FOUND, node->table_name, NULL);
    return SEMANTIC_TABLE_NOT_FOUND;
  }

  return SEMANTIC_SUCCESS;
}

SemanticResult semantic_analyze_insert(Database *db, ASTNode *node) {
  Table *table = semantic_validate_table_exists(db, node->table_name);
  if (!table) {
    SEMANTIC_LOG_ERROR(SEMANTIC_TABLE_NOT_FOUND, node->table_name, NULL);
    return SEMANTIC_TABLE_NOT_FOUND;
  }

  char num_columns_str[12];
  char num_values_str[12];
  snprintf(num_columns_str, sizeof(num_columns_str), "%" PRIu32,
           table->num_columns);
  snprintf(num_values_str, sizeof(num_values_str), "%" PRIu32,
           node->insert_rows.num_values);

  if (table->num_columns != (uint32_t)node->insert_rows.num_values) {
    SEMANTIC_LOG_ERROR(SEMANTIC_COLUMN_COUNT_MISMATCH, num_columns_str,
                       num_values_str);
    return SEMANTIC_COLUMN_COUNT_MISMATCH;
  }

  char *expected_type = NULL;
  char *found_type = NULL;
  char *column_name = NULL;
  SemanticResult result = semantic_validate_data_types(
      table->columns, table->num_columns, node->insert_rows.values,
      &expected_type, &found_type, &column_name);
  if (result != SEMANTIC_SUCCESS) {
    fprintf(stderr,
            "Semantic Error: Type mismatch for column '%s' - expected '%s' but "
            "found '%s'\n",
            column_name, expected_type, found_type);
    return result;
  }

  return SEMANTIC_SUCCESS;
}

// SemanticResult semantic_analyze_select(Database *db, ASTNode *node) {
// 1. existent table
// 2. existent columns

// return SEMANTIC_SUCCESS;
// }
