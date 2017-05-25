
#pragma once

#include "vector.h"
#include "alloc.h"
#include "math.h"

const f32 MAP_MAX_LOAD_FACTOR = 0.9f;

// from Thomas Wang, http://burtleburtle.net/bob/hash/integer.html
inline u32 hash_u32(u32 key) {
    key = (key ^ 61) ^ (key >> 16);
    key = key + (key << 3);
    key = key ^ (key >> 4);
    key = key * 0x27d4eb2d;
    key = key ^ (key >> 15);
    return key;
}

template<typename K, typename V>
struct map_element {
	// TODO(max): test if storing hashes in a separate array performs better
	// TODO(max): use less storage than a bool to signify occupation
	bool occupied = false;
	u32 hash_bucket;
	K key;
	V value;
};

template<typename K, typename V>
struct map {
	vector<map_element<K, V>> contents;
	u32 size	 		= 0;
	allocator* alloc 	= NULL;
	u32 (*hash)(K key) 	= NULL;
	bool use_u32hash = false;
};


template<typename K, typename V> map<K,V> make_map(i32 capacity = 16, u32 (*hash)(K) = NULL);
template<typename K, typename V> map<K,V> make_map(i32 capacity, allocator* a, u32 (*hash)(K) = NULL);
template<typename K, typename V> void destroy_map(map<K,V>* m);
template<typename K, typename V> void map_rehash_elements(map<K,V>* m);
template<typename K, typename V> void map_insert(map<K,V>* m, K key, V value);
template<typename K, typename V> void map_insert_or_find(map<K,V>* m, K key, V value);
template<typename K, typename V> V& map_get(map<K,V>* m, K key);
template<typename K, typename V> V* map_try_get(map<K,V>* m, K key);
template<typename K, typename V> void map_erase(map<K,V>* m, K key);


template<typename K, typename V>
map<K,V> make_map(i32 capacity, u32 (*hash)(K)) {

	return make_map<K,V>(capacity, CURRENT_ALLOC(), hash);
}

template<typename K, typename V>
map<K,V> make_map(i32 capacity, allocator* a, u32 (*hash)(K)) {

	map<K,V> ret;

	ret.alloc 	 = a;
	ret.contents = make_vector<map_element<K,V>>(capacity, ret.alloc);
	if(!hash) {
		assert(sizeof(K) == sizeof(u32));
		ret.use_u32hash = true;
	} else {
		ret.hash = hash;
	}

	return ret;
}

template<typename K, typename V>
void destroy_map(map<K,V>* m) {

	destroy_vector(&m->contents);

	m->size  = 0;
	m->alloc = NULL;
	m->hash  = NULL;
}

template<typename K, typename V>
void map_rehash_elements(map<K,V>* m) {	

}

template<typename K, typename V>
void map_insert(map<K,V>* m, K key, V value) {

	if(m->size >= m->contents.capacity * MAP_MAX_LOAD_FACTOR) {

		vector_grow(&m->contents);

		map_rehash_elements(m); // this is super expensive, avoid at all costs
	}

	map_element<K,V> ele;

	if(m->use_u32hash) {
		ele.hash_bucket = mod(hash_u32(*((u32*)&key)), m->contents.capacity);
	} else {
		ele.hash_bucket = mod((*m->hash)(key), m->contents.capacity);
	}
	ele.key 		= key;
	ele.value 		= value;
	ele.occupied 	= true;

	// robin hood open addressing

	u32 index = ele.hash_bucket;
	u32 probe_length = 0;
	for(;;) {
		if(vector_get(&m->contents, index).occupied) {

			i32 occupied_probe_length = index - vector_get(&m->contents, index).hash_bucket;
			if(occupied_probe_length < 0) {
				occupied_probe_length += m->contents.capacity;
			}

			if((u32)occupied_probe_length < probe_length) {

				map_element<K,V> temp = vector_get(&m->contents, index);
				vector_get(&m->contents, index) = ele;
				ele = temp;

				probe_length = occupied_probe_length;
			} 

			probe_length++;
			index++;
			if (index == m->contents.capacity) {
				index = 0;
			}
		} else {
			vector_get(&m->contents, index) = ele;
			break;
		}
	}
}

template<typename K, typename V>
void map_insert_or_find(map<K,V>* m, K key, V value) {
	
}

template<typename K, typename V>
V& map_get(map<K,V>* m, K key) {
	
}

template<typename K, typename V>
V* map_try_get(map<K,V>* m, K key) {	// can return NULL

}

template<typename K, typename V>
void map_erase(map<K,V>* m, K key) {
	
}
