#include "parser/semantic_analyzer.h"

SemanticResult semantic_validate_columns_exist(Database *db, char *table_name,
                                               char **columns,
                                               int column_count);

SemanticResult semantic_validate_no_duplicate_columns(char **columns,
                                                      int column_count);
