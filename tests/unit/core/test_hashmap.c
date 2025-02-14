#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core/database.h"
#include "core/hashmap.h"
#include "core/table.h"

Table *create_dummy_table(const char *name, uint32_t id) {
  Table *table = malloc(sizeof(Table));
  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';
  table->next_id = id;
  table->num_rows = 0;
  memset(table->pages, 0, sizeof(void *) * TABLE_MAX_PAGES);
  return table;
}

void test_hashmap_free(HashMap *map) {
  if (map == NULL) {
    return;
  }

  for (size_t i = 0; i < map->capacity; i++) {
    Bucket *bucket = map->buckets[i];
    while (bucket != NULL) {
      Bucket *next = bucket->next;
      free(bucket->value);
      free(bucket->key);
      free(bucket);
      bucket = next;
    }
  }

  free(map->buckets);
  free(map);
}

void test_hashmap_initialization() {
  HashMap *map = NULL;
  HashMapResult result = hashmap_initialize(10, &map);

  assert(result == HASHMAP_SUCCESS);
  assert(map != NULL);
  assert(map->capacity == 10);
  assert(map->size == 0);
  assert(map->buckets != NULL);

  test_hashmap_free(map);
  printf("✓ Initialization test passed\n");
}

void test_hashmap_set_and_get() {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  Table *table1 = create_dummy_table("users", 1);
  Table *table2 = create_dummy_table("products", 2);

  // Test basic set
  assert(hashmap_set(map, "users", table1) == HASHMAP_SUCCESS);
  assert(map->size == 1);

  // Test get
  Table *retrieved_table = NULL;
  assert(hashmap_get(map, "users", &retrieved_table) == HASHMAP_SUCCESS);
  assert(strcmp(retrieved_table->name, "users") == 0);

  // Test updating existing key
  Table *new_users_table = create_dummy_table("users_updated", 3);
  assert(hashmap_set(map, "users", new_users_table) == HASHMAP_SUCCESS);
  assert(map->size == 1);

  // Test multiple items
  assert(hashmap_set(map, "products", table2) == HASHMAP_SUCCESS);
  assert(map->size == 2);

  test_hashmap_free(map);
  printf("✓ Set and Get test passed\n");
}

void test_hashmap_delete() {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  Table *table = create_dummy_table("users", 1);
  hashmap_set(map, "users", table);

  // Test successful deletion
  assert(hashmap_delete(map, "users") == HASHMAP_SUCCESS);
  assert(map->size == 0);

  // Test deleting non-existent key
  assert(hashmap_delete(map, "nonexistent") == HASHMAP_KEY_NOT_FOUND);

  test_hashmap_free(map);
  printf("✓ Delete test passed\n");
}

void test_hashmap_resize() {
  HashMap *map = NULL;
  hashmap_initialize(3, &map);

  // Add items until resize is triggered (load factor > 0.7)
  for (int i = 0; i < 3; i++) {
    char key[20];
    sprintf(key, "table%d", i);
    Table *table = create_dummy_table(key, i);
    assert(hashmap_set(map, key, table) == HASHMAP_SUCCESS);
  }

  // Verify that capacity increased
  assert(map->capacity > 3);

  // Verify all items are still accessible
  for (uint32_t i = 0; i < 3; i++) {
    char key[20];
    sprintf(key, "table%d", i);
    Table *retrieved_table = NULL;
    assert(hashmap_get(map, key, &retrieved_table) == HASHMAP_SUCCESS);
    assert(retrieved_table->next_id == i);
  }

  test_hashmap_free(map);
  printf("✓ Resize test passed\n");
}

void test_hashmap_collisions() {
  HashMap *map = NULL;
  hashmap_initialize(1, &map); // Small capacity to force collisions

  // Insert multiple items that will hash to the same bucket
  Table *table1 = create_dummy_table("users", 1);
  Table *table2 = create_dummy_table("settings", 2);
  hashmap_set(map, "users", table1);
  hashmap_set(map, "settings", table2);

  // Verify both items are accessible
  Table *retrieved1 = NULL;
  Table *retrieved2 = NULL;
  assert(hashmap_get(map, "users", &retrieved1) == HASHMAP_SUCCESS);
  assert(hashmap_get(map, "settings", &retrieved2) == HASHMAP_SUCCESS);

  test_hashmap_free(map);
  printf("✓ Collision test passed\n");
}

void test_memory_leaks() {
  printf("Running memory leak tests...\n");

  HashMap *map = NULL;
  hashmap_initialize(5, &map);

  // Test 1: Add and remove multiple items
  for (int i = 0; i < 10; i++) {
    char key[20];
    sprintf(key, "table%d", i);
    Table *table = create_dummy_table(key, i);
    hashmap_set(map, key, table);

    // Delete every other entry
    if (i % 2 == 0) {
      hashmap_delete(map, key);
    }
  }

  // Test 2: Override existing keys
  Table *table1 = create_dummy_table("test", 1);
  Table *table2 = create_dummy_table("test", 2);
  hashmap_set(map, "test", table1);
  hashmap_set(map, "test", table2); // Should free table1

  test_hashmap_free(map);
  printf("✓ Memory leak test completed\n");
}

void test_stress() {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  // Insert large number of items
  for (int i = 0; i < 1000; i++) {
    char key[20];
    sprintf(key, "table%d", i);
    Table *table = create_dummy_table(key, i);
    assert(hashmap_set(map, key, table) == HASHMAP_SUCCESS);
  }

  // Verify random access
  for (int i = 0; i < 100; i++) {
    int random_key = rand() % 1000;
    char key[20];
    sprintf(key, "table%d", random_key);
    Table *retrieved_table = NULL;
    assert(hashmap_get(map, key, &retrieved_table) == HASHMAP_SUCCESS);
  }

  test_hashmap_free(map);
  printf("✓ Stress test passed\n");
}

void test_edge_cases() {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  // Test NULL map
  assert(hashmap_set(NULL, "test", NULL) == HASHMAP_INVALID_MAP);
  assert(hashmap_get(NULL, "test", NULL) == HASHMAP_INVALID_MAP);
  assert(hashmap_delete(NULL, "test") == HASHMAP_INVALID_MAP);

  // Test non-existent key
  Table *retrieved_table = NULL;
  assert(hashmap_get(map, "nonexistent", &retrieved_table) ==
         HASHMAP_KEY_NOT_FOUND);

  hashmap_free(map);
  printf("✓ Edge cases test passed\n");
}

int main() {
  printf("Running hashmap tests...\n");

  test_hashmap_initialization();
  test_hashmap_set_and_get();
  test_hashmap_delete();
  test_hashmap_resize();
  test_hashmap_collisions();
  test_stress();
  test_edge_cases();

  printf("All tests passed!\n");
  return 0;
}
