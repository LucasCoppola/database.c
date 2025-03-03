#include "parser/semantic_analyzer.h"
#include "core/database.h"
#include "parser/ast.h"
#include "utils/semantic_logger.h"

SemanticResult semantic_analyze(Database *db, ASTNode *node) {
  switch (node->type) {
  case NODE_CREATE_TABLE:
    // return semantic_analyze_create_table(db, node);
    break;
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

// SemanticResult semantic_analyze_create_table(Database *db, ASTNode *node) {
// 1. non-existent table
// 2. non-existent column name
// 3. valid column type

// return SEMANTIC_SUCCESS;
// }

SemanticResult semantic_analyze_drop_table(Database *db, ASTNode *node) {
  SemanticResult table_result =
      semantic_validate_table_existance(db, node->table_name);
  if (table_result != SEMANTIC_SUCCESS) {
    SEMANTIC_LOG_ERROR(table_result, node->table_name, NULL);
    return table_result;
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
