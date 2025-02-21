#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "core/database.h"
#include "core/hashmap.h"
#include "utils/logger.h"

HashMapResult hashmap_initialize(size_t capacity, HashMap **out_map) {
  *out_map = malloc(sizeof(HashMap));
  if (*out_map == NULL) {
    return HASHMAP_ALLOCATION_FAILURE;
  }

  (*out_map)->buckets = calloc(capacity, sizeof(Bucket *));
  if ((*out_map)->buckets == NULL) {
    free(*out_map);
    return BUCKETS_ALLOCATION_FAILURE;
  }

  (*out_map)->capacity = capacity;
  (*out_map)->size = 0;
  return HASHMAP_SUCCESS;
}

HashMapResult hashmap_set(HashMap *map, char *key, Table *value) {
  if (map == NULL) {
    return HASHMAP_INVALID_MAP;
  }

  float loadFactor = ((float)map->size + 1) / (float)map->capacity;
  if (loadFactor > 0.7) {
    HashMapResult resize_result = hashmap_resize(map);
    if (resize_result != HASHMAP_SUCCESS) {
      LOG_ERROR("hashmap", "resize", HASHMAP_RESIZE_FAILURE);
      return HASHMAP_RESIZE_FAILURE;
    }
  }

  size_t index = hash(key, map->capacity);
  Bucket *out_bucket = NULL;
  BucketResult find_result = bucket_find(map, index, key, &out_bucket);

  if (find_result == BUCKET_SUCCESS) {
    free(out_bucket->value);
    out_bucket->value = value;
    return HASHMAP_SUCCESS;
  } else if (find_result == BUCKET_NOT_FOUND) {
    Bucket *out_bucket = NULL;
    BucketResult create_result = bucket_create(key, value, &out_bucket);
    if (create_result != BUCKET_SUCCESS) {
      return BUCKETS_ALLOCATION_FAILURE;
    }

    out_bucket->next = map->buckets[index];
    map->buckets[index] = out_bucket;
    map->size++;
    return HASHMAP_SUCCESS;
  } else {
    return HASHMAP_UNKNOWN_ERROR;
  }
};

HashMapResult hashmap_get(HashMap *map, char *key, Table **out_value) {
  if (map == NULL) {
    return HASHMAP_INVALID_MAP;
  }

  size_t index = hash(key, map->capacity);
  Bucket *out_bucket = NULL;
  BucketResult find_result = bucket_find(map, index, key, &out_bucket);
  if (find_result == BUCKET_SUCCESS) {
    *out_value = out_bucket->value;
    return HASHMAP_SUCCESS;
  }

  return HASHMAP_KEY_NOT_FOUND;
};

HashMapResult hashmap_delete(HashMap *map, char *key) {
  if (map == NULL) {
    return HASHMAP_INVALID_MAP;
  }

  size_t index = hash(key, map->capacity);
  Bucket *bucket = map->buckets[index];
  Bucket *prev = NULL;

  while (bucket != NULL) {

    if (strcmp(bucket->key, key) == 0) {
      if (prev == NULL) {
        // bucket is the head
        map->buckets[index] = bucket->next;
      } else {
        prev->next = bucket->next;
      }
      free(bucket->key);
      // free the value/table in the table function
      free(bucket);
      map->size--;
      return HASHMAP_SUCCESS;
    }

    prev = bucket;
    bucket = bucket->next;
  }

  return HASHMAP_KEY_NOT_FOUND;
}

HashMapResult hashmap_free(HashMap *map) {
  if (map == NULL) {
    return HASHMAP_INVALID_MAP;
  }

  for (size_t i = 0; i < map->capacity; i++) {
    Bucket *bucket = map->buckets[i];
    while (bucket != NULL) {
      Bucket *next = bucket->next;
      free(bucket->key);
      // free the value/table in the table function
      free(bucket);
      bucket = next;
    }
  }

  free(map->buckets);
  free(map);
  return HASHMAP_SUCCESS;
};
