#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <stdbool.h>

typedef struct ASTNode ASTNode;
typedef struct Database Database;
typedef struct Value Value;
typedef struct Column Column;
typedef enum DataType DataType;

typedef enum {
    SEMANTIC_SUCCESS,
    SEMANTIC_TABLE_NOT_FOUND,
    SEMANTIC_COLUMN_NOT_FOUND,
    SEMANTIC_TYPE_MISMATCH,
    SEMANTIC_DUPLICATE_TABLE,
    SEMANTIC_DUPLICATE_COLUMN
} SemanticResult;

SemanticResult semantic_analyze(Database *db, ASTNode *node);

SemanticResult semantic_analyze_create_table(Database *db,  ASTNode *node);
SemanticResult semantic_analyze_drop_table(Database *db, ASTNode *node);
SemanticResult semantic_analyze_insert(Database *db, ASTNode *node);
SemanticResult semantic_analyze_select(Database *db, ASTNode *node);

bool semantic_validate_table_exists(Database *db, char *table_name);
bool semantic_validate_columns_unique(Column *columns, int column_count, char **out_column);
SemanticResult semantic_validate_data_types(Database *db, char *table_name, char *column_name, DataType value_type);

#endif
