#include <types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <lib/hashmap.h>

/*
 * The default hash function used by the hash table implementation. Based on the
 * Jenkins Hash Function (http://en.wikipedia.org/wiki/Jenkins_hash_function)
 */
static uint32_t default_hash(char *key, size_t len) {
	uint32_t hash, i;

	for(hash = i = 0; i < len; ++i) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	} 

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

/*
 * Allocates a hashmap that supports 256 different buckets.
 */
hashmap_t *hashmap_allocate() {
	hashmap_t *hashmap = (hashmap_t *) malloc(sizeof(hashmap_t));
	if(!hashmap) {
		return NULL;
	}

	memset(hashmap, 0, sizeof(hashmap_t));
	hashmap->num_buckets = 256;

	// Ugly method to get mask
	hashmap->mask = hashmap->num_buckets - 1;

	// Allocate bucket structures
	hashmap_bucket_t* bucket;
	hashmap_bucket_t* prev_bucket = NULL;

	for(int i = 0; i < hashmap->num_buckets; i++) {
		bucket = (hashmap_bucket_t *) malloc(sizeof(hashmap_bucket_t));
		memset(bucket, 0, sizeof(hashmap_bucket_t));

		// Set the bucket's next structure
		if(likely(prev_bucket)) {
			prev_bucket->next = bucket;
		}

		// If it's the 0th bucket, set it in the hashmap
		if(unlikely(i == 0)) {
			hashmap->buckets = bucket;
		}

		// Store a pointer to this bucket so we can set its next pointer
		prev_bucket = bucket;
	}

	return hashmap;
}

/*
 * Releases the memory associated with a a hashmap, including all bucket and
 * data structures.
 */
void hashmap_release(hashmap_t* map) {
	hashmap_bucket_t* bucket = map->buckets;
	hashmap_data_t* data;

	// Deallocate buckets
	while(likely(bucket != NULL)) {
		data = bucket->data;

		// Deallocate the data in the bucket.
		while(likely(data != NULL)) {
			free(data->key);
			free(data);
			data = data->next;
		}
	}

	// Clear remaining memory
	free(map);
}

/*
 * Inserts an item into the hashmap. If the key already exists, its data is
 * overwritten.
 */
void hashmap_insert(hashmap_t* hashmap, void* key, void* value) {
	// Create a copy of the key.
	size_t keyLength = strlen(key);
	void* keyCopy = (void *) malloc(keyLength+1);

	memset(keyCopy, 0, keyLength+1);
	memcpy(keyCopy, key, keyLength);

	// Calculate hash
	uint32_t hash = default_hash(keyCopy, keyLength);
	hash &= hashmap->mask;

	// Locate the bucket in the hashmap.
	hashmap_bucket_t *bucket = hashmap->buckets;

	for(uint32_t i = 0; i < hash; i++) {
		// Go to the next bucket.
		bucket = bucket->next;

		if(bucket == NULL) return;
	}

	// Search through all data in the bucket to see if the key exists
	hashmap_data_t* data;
	hashmap_data_t* emptyData;

	data = bucket->data;

	// If there's no data structure in this bucket, create some.
	if(unlikely(data == NULL)) {
		hashmap_data_t* newData = (hashmap_data_t *) malloc(sizeof(hashmap_data_t));
		memset(newData, 0, sizeof(hashmap_data_t));

		newData->data = value;
		newData->key = keyCopy;

		// Set the bucket's data structure
		bucket->data = newData;

		return;
	} else {
		// Search through the data structures instead.
		while(likely(data != NULL)) {
			if(data->key == NULL) {
				emptyData = value;
			}

			// Do the keys match?
			if(unlikely(memcmp(keyCopy, data->key, keyLength) == 0)) {
				// Release old data.
				free(data->data);

				// Set data.
				data->data = value;

				// We're done here.
				return;
			}
		}

		// If we have an empty data structure, stuff our data into it.
		if(emptyData) {
			emptyData->key = keyCopy;
			emptyData->data = value;
			return;
		} else { // We need to allocate a data structure
			hashmap_data_t* newData = (hashmap_data_t *) malloc(sizeof(hashmap_data_t));
			memset(newData, 0, sizeof(hashmap_data_t));

			newData->data = value;
			newData->key = keyCopy;

			// Set it as the next data structure.
			data->next = newData;
			return;
		}
	}
}

/*
 * Retrieves an item from the hashmap, or returns NULL if not found.
 */
void* hashmap_get(hashmap_t* hashmap, void* key) {
	// Calculate hash
	size_t keyLength = strlen(key);
	uint32_t hash = default_hash(key, keyLength);
	hash &= hashmap->mask;

	// Locate the bucket in the hashmap.
	hashmap_bucket_t *bucket = hashmap->buckets;

	for(uint32_t i = 0; i < hash; i++) {
		// Go to the next bucket.
		bucket = bucket->next;
	}

	// Loop through the data structures in the bucket
	hashmap_data_t* data = bucket->data;

	while(likely(data != NULL)) {
		// We've found the key.
		if(unlikely(memcmp(key, data->key, keyLength) == 0)) {
			return data->data;
		}

		data = data->next;
	}

	// We didn't find the key in the bucket.
	return NULL;
}

/*
 * Removes an entry from the hashmap, releasing the memory associated with the
 * key as well.
 */
int hashmap_delete(hashmap_t* hashmap, void* key) {
	// Calculate hash
	size_t keyLength = strlen(key);
	uint32_t hash = default_hash(key, keyLength);
	hash &= hashmap->mask;

	// Locate the bucket in the hashmap.
	hashmap_bucket_t *bucket = hashmap->buckets;

	for(uint32_t i = 0; i < hash; i++) {
		// Go to the next bucket.
		bucket = bucket->next;
	}

	// Loop through the data structures in the bucket
	hashmap_data_t* data = bucket->data;

	while(likely(data != NULL)) {
		// We've found the key, so delete it.
		if(unlikely(memcmp(key, data->key, keyLength) == 0)) {
			free(data->key);
			data->data = NULL;
			return 0;
		}

		data = data->next;
	}

	// The key couldn't be found in the hashmap
	return ENOTFOUND;
}
