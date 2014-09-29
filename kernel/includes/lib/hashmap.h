#ifndef HASHMAP_H
#define HASHMAP_H

#include <types.h>

/*
 * This structure actually contains the key and data, and forms a linked list
 * of data in a certain bucket.
 *
 * If the key pointer is NULL, this is a structure that has been deleted, and
 * can thus be re-used if we need to insert more data whose hash would place
 * it in this bucket.
 */
typedef struct hashmap_data {
	void* key;
	void* data;

	struct hashmap_data *next;
} hashmap_data_t;

/*
 * Each hash maps to one of these hashmap_bucket structures, which contains a
 * pointer to a copy of the key and data, as well as a pointer to the next
 * data structure in this bucket, which is also of the type hashmap_bucket.
 *
 * If the next pointer is NULL, this is the last data in the bucket.
 */
typedef struct hashmap_bucket {
	hashmap_data_t *data;

	struct hashmap_bucket* next;
} hashmap_bucket_t;

/*
 * This type contains pointers to all buckets as well as other useful variables
 * needed to make a hashmap work.
 */
typedef struct hashmap {
	hashmap_bucket_t *buckets;

	int seed;
	int num_buckets;

	uint32_t mask;
} hashmap_t;

// Initialisation and deallocation
hashmap_t *hashmap_allocate();
void hashmap_release(hashmap_t*);

// Hashmap's content manipulation
void hashmap_insert(hashmap_t*, void*, void*);
void* hashmap_get(hashmap_t*, void*);
int hashmap_delete(hashmap_t*, void*);

#endif