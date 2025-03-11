#include <stdbool.h>
#include <stdio.h>

#include "statements/test_create_table.c"
#include "statements/test_drop_table.c"
#include "statements/test_insert.c"
#include "statements/test_select.c"

int main() {
  UNITY_BEGIN();

  printf("Running CREATE TABLE tests...\n");
  run_create_table_tests();

  printf("Running DROP TABLE tests...\n");
  run_drop_table_tests();

  printf("Running SELECT tests...\n");
  run_select_tests();

  printf("Running INSERT tests...\n");
  run_insert_tests();

  return UNITY_END();
}
