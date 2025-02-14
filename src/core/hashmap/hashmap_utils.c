#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/hashmap.h"

size_t hash(const char *key, size_t capacity) {
  size_t hash = 5381;
  int c;

  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash % capacity;
}

BucketResult bucket_find(HashMap *map, size_t idx, char *key,
                         Bucket **out_bucket) {
  Bucket *bucket = map->buckets[idx];
  while (bucket != NULL) {
    if (strcmp(bucket->key, key) == 0) {
      *out_bucket = bucket;
      return BUCKET_SUCCESS;
    }

    bucket = bucket->next;
  }

  return BUCKET_NOT_FOUND;
}

BucketResult bucket_create(char *key, Table *value, Bucket **out_bucket) {
  Bucket *bucket = malloc(sizeof(Bucket));
  if (bucket == NULL) {
    return BUCKET_ALLOCATION_FAILURE;
  }

  bucket->key = strdup(key);
  bucket->value = value;
  bucket->next = NULL;
  *out_bucket = bucket;
  return BUCKET_SUCCESS;
}

HashMapResult hashmap_resize(HashMap *map) {
  size_t newCapacity = map->capacity * 2;
  Bucket **newBuckets = calloc(newCapacity, sizeof(Bucket *));
  if (newBuckets == NULL) {
    return HASHMAP_RESIZE_FAILURE;
  }

  for (size_t i = 0; i < map->capacity; i++) {
    Bucket *bucket = map->buckets[i];
    while (bucket != NULL) {
      Bucket *next = bucket->next;
      size_t newIndex = hash(bucket->key, newCapacity);

      bucket->next = newBuckets[newIndex];
      newBuckets[newIndex] = bucket;

      bucket = next;
    }
  }

  free(map->buckets);
  map->buckets = newBuckets;
  map->capacity = newCapacity;
  return HASHMAP_SUCCESS;
}
