#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "core/database.h"
#include "core/table.h"

#include "parser/ast.h"
#include "parser/semantic_analyzer.h"
#include "utils/convertions.h"
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
    return semantic_analyze_select(db, node);
  case NODE_DELETE:
    return semantic_analyze_delete(db, node);
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

  if (strlen(node->table_name) >= MAX_NAME_LENGTH) {
    SEMANTIC_LOG_ERROR(SEMANTIC_INVALID_NAME_LENGTH, node->table_name, NULL);
    return SEMANTIC_INVALID_NAME_LENGTH;
  }

  char *out_column = NULL;
  if (!semantic_validate_columns_uniqueness(node->create_table.columns,
                                            node->create_table.num_columns,
                                            &out_column)) {
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

  char *out_expected_type = NULL;
  char *out_found_type = NULL;
  char *out_column_name = NULL;
  SemanticResult result = semantic_validate_data_types(
      table->columns, table->num_columns, node->insert_rows.values,
      &out_expected_type, &out_found_type, &out_column_name);
  if (result != SEMANTIC_SUCCESS) {
    fprintf(stderr,
            "Semantic Error: Type mismatch for column '%s' - expected '%s' but "
            "found '%s'\n",
            out_column_name, out_expected_type, out_found_type);
    return result;
  }

  return SEMANTIC_SUCCESS;
}

SemanticResult semantic_analyze_select(Database *db, ASTNode *node) {
  Table *table = semantic_validate_table_exists(db, node->table_name);
  if (!table) {
    SEMANTIC_LOG_ERROR(SEMANTIC_TABLE_NOT_FOUND, node->table_name, NULL);
    return SEMANTIC_TABLE_NOT_FOUND;
  }

  if (has_where_condition(node)) {
    SemanticResult semantic_result =
        semantic_analyze_where_condition(table, node->where_condition);
    if (semantic_result != SEMANTIC_SUCCESS) {
      return semantic_result;
    }
  }

  if (node->select_rows.select_all) {
    return SEMANTIC_SUCCESS;
  }

  char *out_column = NULL;
  if (!semantic_validate_select_columns(table, node, &out_column)) {
    SEMANTIC_LOG_ERROR(SEMANTIC_COLUMN_NOT_FOUND, out_column, NULL);
    return SEMANTIC_COLUMN_NOT_FOUND;
  }

  return SEMANTIC_SUCCESS;
}

SemanticResult semantic_analyze_delete(Database *db, ASTNode *node) {
  Table *table = semantic_validate_table_exists(db, node->table_name);
  if (!table) {
    SEMANTIC_LOG_ERROR(SEMANTIC_TABLE_NOT_FOUND, node->table_name, NULL);
    return SEMANTIC_TABLE_NOT_FOUND;
  }

  if (has_where_condition(node)) {
    SemanticResult semantic_result =
        semantic_analyze_where_condition(table, node->where_condition);
    if (semantic_result != SEMANTIC_SUCCESS) {
      return semantic_result;
    }
  }

  return SEMANTIC_SUCCESS;
}

SemanticResult
semantic_analyze_where_condition(Table *table, WhereCondition where_condition) {
  Column *col = semantic_find_column(table, where_condition.column_name);
  if (col == NULL) {
    SEMANTIC_LOG_ERROR(SEMANTIC_COLUMN_NOT_FOUND, where_condition.column_name,
                       NULL);
    return SEMANTIC_COLUMN_NOT_FOUND;
  }

  if (col->type != where_condition.value->type) {
    fprintf(stderr, "Semantic Error: Cannot compare '%s' with '%s'\n",
            data_type_to_string(col->type),
            data_type_to_string(where_condition.value->type));
    return SEMANTIC_TYPE_MISMATCH;
  }

  if (!semantic_is_operator_compatible(col->type, where_condition.op)) {
    fprintf(stderr, "Invalid operator: cannot use '%c' with '%s'\n",
            where_condition.op, data_type_to_string(col->type));
    return SEMANTIC_INVALID_OPERATOR;
  }

  return SEMANTIC_SUCCESS;
}
