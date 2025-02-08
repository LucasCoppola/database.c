#include <stdbool.h>
#include <stdio.h>

#include "test_create_table.c"
#include "test_drop_table.c"
#include "test_insert.c"
#include "test_select.c"

int main() {
  printf("Running CREATE TABLE tests...\n");
  run_create_table_tests();

  printf("Running DROP TABLE tests...\n");
  run_drop_table_tests();

  printf("Running SELECT tests...\n");
  run_select_tests();

  printf("Running INSERT tests...\n");
  run_insert_tests();

  return 0;
}
