#ifndef STORAGE_H
#define STORAGE_H

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

#include <stdint.h>

void serialize_row(Row *source, void *destination); 
void deserialize_row(void *source, Row *destination);
void *get_row_slot(Table *table, uint32_t row_num);

extern const uint32_t TABLE_MAX_ROWS;  
extern const uint32_t ROWS_PER_PAGE;  
extern const uint32_t ROW_SIZE ;  

#endif
