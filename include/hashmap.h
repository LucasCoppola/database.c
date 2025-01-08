#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>

// Forward declaration of Table to avoid circular dependency
typedef struct Table Table;

typedef enum {
  HASHMAP_SUCCESS,
  HASHMAP_ALLOCATION_FAILURE,
  BUCKETS_ALLOCATION_FAILURE,
  HASHMAP_RESIZE_FAILURE,
  HASHMAP_KEY_NOT_FOUND,
  HASHMAP_INVALID_MAP,
  HASHMAP_UNKNOWN_ERROR
} HashMapResult;

typedef enum {
  BUCKET_SUCCESS,
  BUCKET_NOT_FOUND,
  BUCKET_ALLOCATION_FAILURE
} BucketResult;

typedef struct Bucket {
  char *key;
  Table *value;
  struct Bucket *next;
} Bucket;

typedef struct {
  Bucket **buckets;
  size_t capacity;
  size_t size;
} HashMap;

HashMapResult hashmap_initialize(size_t capacity, HashMap **out_map);
HashMapResult hashmap_set(HashMap *map, char *key, Table *value);
HashMapResult hashmap_get(HashMap *map, char *key, Table **out_value);
HashMapResult hashmap_delete(HashMap *map, char *key);
HashMapResult hashmap_free(HashMap *map);

// hashmap_utils
size_t hash(const char *key, size_t capacity);
HashMapResult hashmap_resize(HashMap *map);
BucketResult bucket_find(HashMap *map, size_t idx, char *key, Bucket **out_bucket);
BucketResult bucket_create(char *key, Table *value, Bucket **out_bucket);

#endif
