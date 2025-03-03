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
    // return semantic_analyze_insert(db, node);
    break;
  case NODE_SELECT:
    // return semantic_analyze_select(db, node);
    break;
  default:
    return SEMANTIC_SUCCESS;
  }
  return SEMANTIC_SUCCESS;
}

SemanticResult semantic_analyze_create_table(Database *db, ASTNode *node) {
  bool table_exists = semantic_validate_table_exists(db, node->table_name);
  if (table_exists) {
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
  bool table_exists = semantic_validate_table_exists(db, node->table_name);
  if (!table_exists) {
    SEMANTIC_LOG_ERROR(SEMANTIC_TABLE_NOT_FOUND, node->table_name, NULL);
    return SEMANTIC_TABLE_NOT_FOUND;
  }

  return SEMANTIC_SUCCESS;
}

// SemanticResult semantic_analyze_insert(Database *db, ASTNode *node) {
// 1. existent table
// 2. number of columns match (given that we have the syntax where we dont
// specify the columns)
// 3. column type matches

// return SEMANTIC_SUCCESS;
// }

// SemanticResult semantic_analyze_select(Database *db, ASTNode *node) {
// 1. existent table
// 2. existent columns

// return SEMANTIC_SUCCESS;
// }
