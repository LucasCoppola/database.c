#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

typedef struct ASTNode ASTNode;
typedef struct Database Database;
typedef struct Value Value;
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

SemanticResult semantic_validate_table_existance(Database *db, char *table_name);
SemanticResult semantic_validate_columns_existance(Database *db, char *table_name, char **columns, int column_count);
SemanticResult semantic_validate_data_types(Database *db, char *table_name, char *column_name, DataType value_type);
SemanticResult semantic_validate_no_duplicate_tables(Database *db, char *table_name);
SemanticResult semantic_validate_no_duplicate_columns(char **columns, int column_count);

#endif
