#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <stdbool.h>
#include "core/table.h"
#include "core/row.h"

typedef struct WhereCondition WhereCondition;
typedef struct ASTNode ASTNode;
typedef struct Database Database;

typedef enum {
    SEMANTIC_SUCCESS,
    SEMANTIC_TABLE_NOT_FOUND,
    SEMANTIC_COLUMN_NOT_FOUND,
    SEMANTIC_TYPE_MISMATCH,
    SEMANTIC_DUPLICATE_TABLE,
    SEMANTIC_DUPLICATE_COLUMN,
    SEMANTIC_COLUMN_COUNT_MISMATCH,
    SEMANTIC_INVALID_NAME_LENGTH,
    SEMANTIC_INVALID_OPERATOR
} SemanticResult;

SemanticResult semantic_analyze(Database *db, ASTNode *node);

SemanticResult semantic_analyze_create_table(Database *db,  ASTNode *node);
SemanticResult semantic_analyze_drop_table(Database *db, ASTNode *node);
SemanticResult semantic_analyze_insert(Database *db, ASTNode *node);
SemanticResult semantic_analyze_select(Database *db, ASTNode *node);
SemanticResult semantic_analyze_delete(Database *db, ASTNode *node);
SemanticResult semantic_analyze_where_condition(Table *table, WhereCondition where_condition);

Table *semantic_validate_table_exists(Database *db, char *table_name);

bool semantic_validate_columns_uniqueness(Column *columns, int column_count, char **out_column);
bool semantic_validate_select_columns(Table *table, ASTNode *node, char **out_column);
Column *semantic_find_column(Table *table, const char *column_name);

SemanticResult semantic_validate_data_types(Column *columns, int num_columns, Value **values, 
                                              char **expected_type, char **found_type, char **column_name);
bool semantic_is_operator_compatible(DataType col_type, char op);

#endif
