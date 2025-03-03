#include "core/table.h"
#include "parser/semantic_analyzer.h"

SemanticResult semantic_validate_data_types(Database *db, char *table_name,
                                            char *column_name,
                                            DataType value_type);
