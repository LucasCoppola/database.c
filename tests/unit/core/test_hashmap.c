#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/database.h"
#include "core/hashmap.h"
#include "core/table.h"
#include "libs/unity.h"

void setUp(void) {}

void tearDown(void) {}

Table *create_dummy_table(const char *name, uint32_t id) {
  Table *table = malloc(sizeof(Table));
  strncpy(table->name, name, MAX_NAME_LENGTH - 1);
  table->name[MAX_NAME_LENGTH - 1] = '\0';
  table->next_id = id;
  table->num_rows = 0;
  memset(table->pages, 0, sizeof(void *) * TABLE_MAX_PAGES);
  return table;
}

void mock_hashmap_free(HashMap *map) {
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

void test_hashmap_set_and_get(void) {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  Table *table1 = create_dummy_table("users", 1);
  Table *table2 = create_dummy_table("products", 2);

  // Test basic set
  TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_set(map, "users", table1));
  TEST_ASSERT_EQUAL(1, map->size);

  // Test get
  Table *retrieved_table = NULL;
  TEST_ASSERT_EQUAL(HASHMAP_SUCCESS,
                    hashmap_get(map, "users", &retrieved_table));
  TEST_ASSERT_EQUAL_STRING("users", retrieved_table->name);

  // Test updating existing key
  Table *new_users_table = create_dummy_table("users_updated", 3);
  TEST_ASSERT_EQUAL(HASHMAP_SUCCESS,
                    hashmap_set(map, "users", new_users_table));
  TEST_ASSERT_EQUAL(1, map->size);

  // Test multiple items
  TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_set(map, "products", table2));
  TEST_ASSERT_EQUAL(2, map->size);

  mock_hashmap_free(map);
}

void test_hashmap_delete(void) {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  Table *table = create_dummy_table("users", 1);
  hashmap_set(map, "users", table);

  // Test successful deletion
  TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_delete(map, "users"));
  TEST_ASSERT_EQUAL(0, map->size);
  free(table);

  // Test deleting non-existent key
  TEST_ASSERT_EQUAL(HASHMAP_KEY_NOT_FOUND, hashmap_delete(map, "nonexistent"));

  mock_hashmap_free(map);
}

void test_hashmap_resize(void) {
  HashMap *map = NULL;
  hashmap_initialize(3, &map);

  // Add items until resize is triggered (load factor > 0.7)
  for (int i = 0; i < 3; i++) {
    char key[20];
    sprintf(key, "table%d", i);
    Table *table = create_dummy_table(key, i);
    TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_set(map, key, table));
  }

  TEST_ASSERT_GREATER_THAN(3, map->capacity);

  // Verify all items are still accessible
  for (uint32_t i = 0; i < 3; i++) {
    char key[20];
    sprintf(key, "table%d", i);
    Table *retrieved_table = NULL;
    TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_get(map, key, &retrieved_table));
    TEST_ASSERT_EQUAL(i, retrieved_table->next_id);
  }

  mock_hashmap_free(map);
}

void test_hashmap_collisions(void) {
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
  TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_get(map, "users", &retrieved1));
  TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_get(map, "settings", &retrieved2));

  mock_hashmap_free(map);
}

void test_stress(void) {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  // Insert large number of items
  for (int i = 0; i < 1000; i++) {
    char key[20];
    sprintf(key, "table%d", i);
    Table *table = create_dummy_table(key, i);
    TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_set(map, key, table));
  }

  // Verify random access
  for (int i = 0; i < 100; i++) {
    int random_key = rand() % 1000;
    char key[20];
    sprintf(key, "table%d", random_key);
    Table *retrieved_table = NULL;
    TEST_ASSERT_EQUAL(HASHMAP_SUCCESS, hashmap_get(map, key, &retrieved_table));
  }

  mock_hashmap_free(map);
}

void test_edge_cases(void) {
  HashMap *map = NULL;
  hashmap_initialize(10, &map);

  // Test NULL map
  TEST_ASSERT_EQUAL(HASHMAP_INVALID_MAP, hashmap_set(NULL, "test", NULL));
  TEST_ASSERT_EQUAL(HASHMAP_INVALID_MAP, hashmap_get(NULL, "test", NULL));
  TEST_ASSERT_EQUAL(HASHMAP_INVALID_MAP, hashmap_delete(NULL, "test"));

  // Test non-existent key
  Table *retrieved_table = NULL;
  TEST_ASSERT_EQUAL(HASHMAP_KEY_NOT_FOUND,
                    hashmap_get(map, "nonexistent", &retrieved_table));

  mock_hashmap_free(map);
}

// Main function to run all tests
int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_hashmap_set_and_get);
  RUN_TEST(test_hashmap_delete);
  RUN_TEST(test_hashmap_resize);
  RUN_TEST(test_hashmap_collisions);
  RUN_TEST(test_stress);
  RUN_TEST(test_edge_cases);

  return UNITY_END();
}
