#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../include/database.h"

void test_database_creation() {
  printf("\n=== Testing Database Creation ===\n");
  Database *db = create_database();
  assert(db != NULL);
  assert(db->tables_head == NULL);
  free_database(db);
  printf("✓ Database created and freed successfully\n");
}

void test_table_creation() {
  printf("\n=== Testing Table Creation ===\n");
  Database *db = create_database();

  // Test 1: Normal table creation
  Table *table1 = create_table(db, "Users");
  assert(table1 != NULL);
  assert(strcmp(table1->name, "Users") == 0);
  assert(table1->size == 0);
  assert(table1->rows_head == NULL);
  assert(table1->rows_tail == NULL);
  printf("✓ Normal table creation successful\n");

  // Test 2: Table with too long name (should fail gracefully)
  char long_name[MAX_NAME_LENGTH + 10];
  memset(long_name, 'a', MAX_NAME_LENGTH + 5);
  long_name[MAX_NAME_LENGTH + 5] = '\0';
  Table *table2 = create_table(db, long_name);
  assert(table2 == NULL);
  printf("✓ Long table name correctly rejected\n");

  // Test 3: Null database (should fail gracefully)
  Table *table3 = create_table(NULL, "Test");
  assert(table3 == NULL);
  printf("✓ Null database correctly rejected\n");

  free_database(db);
}

void test_row_operations() {
  printf("\n=== Testing Row Operations ===\n");
  Database *db = create_database();
  Table *table = create_table(db, "Users");

  // Test 1: Insert first row
  printf("\nInserting first row...\n");
  insert_row(table, "Alice", 25);
  assert(table->size == 1);
  assert(table->rows_head != NULL);
  assert(table->rows_head->id == 1);
  assert(strcmp(table->rows_head->name, "Alice") == 0);
  assert(table->rows_head->age == 25);
  printf("✓ First row inserted correctly\n");

  // Test 2: Insert second row
  printf("\nInserting second row...\n");
  insert_row(table, "Bob", 30);
  assert(table->size == 2);
  assert(table->rows_head->next != NULL);
  assert(table->rows_head->next->id == 2);
  printf("✓ Second row inserted correctly\n");

  // Test 3: Delete first row
  printf("\nDeleting first row...\n");
  delete_row(table, 1);
  assert(table->size == 1);
  assert(strcmp(table->rows_head->name, "Bob") == 0);
  printf("✓ First row deleted successfully\n");

  // Test 4: Try to delete non-existent row
  printf("\nTrying to delete non-existent row (should fail gracefully)...\n");
  delete_row(table, 999);
  assert(table->size == 1);
  printf("✓ Non-existent row deletion handled correctly\n");

  free_database(db);
}

void test_edge_cases() {
  printf("\n=== Testing Edge Cases ===\n");
  Database *db = create_database();
  Table *table = create_table(db, "Test");

  // Test 1: Empty table selection
  printf("\nSelecting from empty table...\n");
  select_from_table(table);
  printf("✓ Empty table handled correctly\n");

  // Test 2: Row with too long name
  printf("\nTrying to insert row with too long name (should fail "
         "gracefully)...\n");
  char long_name[MAX_NAME_LENGTH + 10];
  memset(long_name, 'a', MAX_NAME_LENGTH + 5);
  long_name[MAX_NAME_LENGTH + 5] = '\0';
  insert_row(table, long_name, 25);
  assert(table->size == 0);
  printf("✓ Long row name correctly rejected\n");

  free_database(db);
}

int main() {
  printf("Starting database tests...\n");

  test_database_creation();
  test_table_creation();
  test_row_operations();
  test_edge_cases();

  printf("\n✓ All tests completed successfully!\n");
  return 0;
}
