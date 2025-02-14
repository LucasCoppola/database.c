#include <stdbool.h>

#include "core/hashmap.h"

HashMapIterator *hashmap_iterator_init(HashMap *map) {
  if (map == NULL) {
    return NULL;
  }

  HashMapIterator *iterator = malloc(sizeof(HashMapIterator));
  if (iterator == NULL) {
    return NULL;
  }

  iterator->map = map;
  iterator->bucket = map->buckets[0];
  iterator->index = 0;
  return iterator;
}

bool hashmap_iterator_has_next(HashMapIterator *iterator) {
  if (iterator->bucket && iterator->bucket->next) {
    return true;
  }

  for (size_t i = iterator->index + 1; i < iterator->map->capacity; i++) {
    if (iterator->map->buckets[i] != NULL) {
      return true;
    }
  }

  return false;
}

Bucket *hashmap_iterator_next(HashMapIterator *iterator) {
  if (iterator->bucket) {
    Bucket *curr_bucket = iterator->bucket;
    iterator->bucket = iterator->bucket->next;
    if (iterator->bucket) {
      return curr_bucket;
    }
  }

  for (iterator->index++; iterator->index < iterator->map->capacity;
       iterator->index++) {
    if (iterator->map->buckets[iterator->index] != NULL) {
      iterator->bucket = iterator->map->buckets[iterator->index];
      return iterator->bucket;
    }
  }

  return NULL;
}

void hashmap_iterator_free(HashMapIterator *iterator) { free(iterator); }
