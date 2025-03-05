#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <stdbool.h>
#include "core/table.h"
#include "core/row.h"

typedef struct ASTNode ASTNode;
typedef struct Database Database;

typedef enum {
    SEMANTIC_SUCCESS,
    SEMANTIC_TABLE_NOT_FOUND,
    SEMANTIC_COLUMN_NOT_FOUND,
    SEMANTIC_TYPE_MISMATCH,
    SEMANTIC_DUPLICATE_TABLE,
    SEMANTIC_DUPLICATE_COLUMN,
    SEMANTIC_COLUMN_COUNT_MISMATCH
} SemanticResult;

SemanticResult semantic_analyze(Database *db, ASTNode *node);

SemanticResult semantic_analyze_create_table(Database *db,  ASTNode *node);
SemanticResult semantic_analyze_drop_table(Database *db, ASTNode *node);
SemanticResult semantic_analyze_insert(Database *db, ASTNode *node);
SemanticResult semantic_analyze_select(Database *db, ASTNode *node);

Table *semantic_validate_table_exists(Database *db, char *table_name);
bool semantic_validate_columns_uniqueness(Column *columns, int column_count, char **out_column);
bool semantic_validate_insert_columns(Table *table, ASTNode *node, char **out_column);
bool semantic_validate_select_columns(Table *table, ASTNode *node, char **out_column);
SemanticResult semantic_validate_data_types(Column *columns, int num_columns, Value *values, 
                                              char **expected_type, char **found_type, char **column_name);

#endif
