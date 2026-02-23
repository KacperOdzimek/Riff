/*
    Hash Map
    Hash map implementation with linear probing.
    Allow for avg. O(1) access to elements by it's keys.
    O(n) memory complexity
*/

/*
    T macro pattern
        [instance name], 
        [key type],    [key type destructor (opt)],
        [stored type], [stored type destructor (opt)],
        [key type hash function - size_t(func)(const KEY*)]
        [key type equal function - int(func)(const KEY* a, const KEY* b) (non-0 if equal)]
*/

#include "generic.h"

/*
    Unpack and Helpers
*/

#define INSTANCE RIFF_FIRST(T)
#define KEY      RIFF_SECOND(T)
#define KEY_DEST RIFF_THIRD(T)
#define VAL      RIFF_FOURTH(T)
#define VAL_DEST RIFF_FIFTH(T)
#define HASH     RIFF_SIXTH(T)
#define EQUAL    RIFF_SEVENTH(T)

#define HASH_NONE 0
#define HASH_FULL 1
#define HASH_TOMB 2

#define INIT_CAPC 16

/*
    Typedef
*/

#ifndef hashmap
    #define hashmap(inst) RIFF_NAME(hashmap, INSTANCE)
#endif

typedef struct hashmap(INSTANCE) {
    char*   priv_used;
    KEY*    priv_keys;
    VAL*    priv_values;
    size_t  priv_size; // actual count of items within
    size_t  priv_capc; // size of arrays
} hashmap(INSTANCE);

/*
    Zero / Destruction
*/

void RIFF_INST(hashmap_zero, INSTANCE)(hashmap(INSTANCE)* tar) {
    tar->priv_used   = 0;
    tar->priv_keys   = 0;
    tar->priv_values = 0;
    tar->priv_size   = 0;
    tar->priv_capc   = 0;
}

#ifndef hashmap_zero
    // Makes unitialized memory proper 0-initialized empty hashmap
    // Does not free anything
    #define hashmap_zero(inst) RIFF_INST(hashmap_zero, inst)
#endif

void RIFF_INST(hashmap_destroy, INSTANCE)(hashmap(INSTANCE) *tar) {
    // call destructors
    for (size_t i = 0; i < tar->priv_capc; i++) {
        if (tar->priv_used[i] == HASH_FULL) {
            KEY_DEST(&tar->priv_keys[i]);
            VAL_DEST(&tar->priv_values[i]);
        }
    }

    // free memory
    if (tar->priv_used)   RIFF_FREE(tar->priv_used);
    if (tar->priv_keys)   RIFF_FREE(tar->priv_keys);
    if (tar->priv_values) RIFF_FREE(tar->priv_values);

    hashmap_zero(INSTANCE)(tar);
}

#ifndef hashmap_destroy
    // Frees hashmap and its keys and values
    // O(n)
    #define hashmap_destroy(inst) RIFF_INST(hashmap_destroy, inst)
#endif

/*
    Memory
*/

int RIFF_INTR(hashmap_alloc, INSTANCE)(hashmap(INSTANCE)* tar, size_t cap) {
    tar->priv_size = 0;
    tar->priv_capc = cap;

    tar->priv_used   = (char*)RIFF_ALLOC(tar->priv_capc * sizeof(char));
    tar->priv_keys   = (KEY*) RIFF_ALLOC(tar->priv_capc * sizeof(KEY));
    tar->priv_values = (VAL*) RIFF_ALLOC(tar->priv_capc * sizeof(VAL));

    if (!tar->priv_used || !tar->priv_keys || !tar->priv_values) {
        if (tar->priv_used)   RIFF_FREE(tar->priv_used);
        if (tar->priv_keys)   RIFF_FREE(tar->priv_keys);
        if (tar->priv_values) RIFF_FREE(tar->priv_values);
        return ERR;
    }

    for (size_t i = 0; i < tar->priv_capc; ++i) tar->priv_used[i] = HASH_NONE;
    return SCC;
}

int RIFF_INST(hashmap_push, INSTANCE)(hashmap(INSTANCE)* tar, KEY key, VAL value); // forward

int RIFF_INST(hashmap_rehash, INSTANCE)(hashmap(INSTANCE)* tar, size_t new_capacity) {
    // null state now, just alloc
    if (tar->priv_capc == 0) return RIFF_INTR(hashmap_alloc, INSTANCE)(tar, new_capacity);

    // alloc new map
    if (new_capacity < tar->priv_size) return ERR;
    hashmap(INSTANCE) new_map; if (RIFF_INTR(hashmap_alloc, INSTANCE)(&new_map, new_capacity) == ERR) return ERR;

    // reinsert items into new map
    for (size_t i = 0; i < tar->priv_capc; ++i) {
        if (tar->priv_used[i] != HASH_FULL) continue;
        int scc = RIFF_INST(hashmap_push, INSTANCE)(&new_map, tar->priv_keys[i], tar->priv_values[i]);
        
        // if failed to insert, destroy partialy created nao
        if (scc != SCC) {
            RIFF_INST(hashmap_destroy, INSTANCE)(&new_map);
            return ERR;
        }
    }

    // delete old arrays
    // do not use hashmap_destroy not to call destructors
    RIFF_FREE(tar->priv_used);
    RIFF_FREE(tar->priv_keys);
    RIFF_FREE(tar->priv_values);

    // if everything succeded move new map into old map
    *tar = new_map;
    return SCC;
}

#ifndef hashmap_rehash
    // Rebuild iternal arrays inside hashmap
    // May fail (new_capacity to small to fit, or allocation failure), O(n)
    #define hashmap_rehash(inst) RIFF_INST(hashmap_rehash, inst)
#endif

/*
    Operations
*/

int RIFF_INST(hashmap_push, INSTANCE)(hashmap(INSTANCE)* tar, KEY key, VAL value) {
    // If none memory assigned, allocate
    if (tar->priv_capc == 0) {
        int scc = RIFF_INTR(hashmap_alloc, INSTANCE)(tar, INIT_CAPC);
        if (scc == ERR) return ERR; // allocation failed
    }

    // Double memory if load factor exceeds 0.7
    if ((tar->priv_size + 1) * 10 > tar->priv_capc * 7) {
        // grow, if grow fails try to fit anyway - there still may be some free spots in the array
        RIFF_INST(hashmap_rehash, INSTANCE)(tar, tar->priv_capc * 2);
    }

    size_t idx = HASH(&key) % tar->priv_capc;
    size_t first_tombstone = (size_t)(-1);

    for (size_t i = 0; i < tar->priv_capc; ++i) {
        size_t pos = (idx + i) % tar->priv_capc;

        // check if key is exactly the same, if so replace value
        if (tar->priv_used[pos] == HASH_FULL && EQUAL(&tar->priv_keys[pos], &key)) {
            KEY_DEST(&tar->priv_keys[pos]);   // free old key
            VAL_DEST(&tar->priv_values[pos]); // free old value
            tar->priv_keys[pos]   = key;
            tar->priv_values[pos] = value;
            return SCC;
        }
        // remember first tombstone
        else if (tar->priv_used[pos] == HASH_TOMB) {
            if (first_tombstone == (size_t)(-1)) first_tombstone = pos; // remember first tombstone
        }
        // insert at first tombstone if available, else at empty slot
        else if (tar->priv_used[pos] == HASH_NONE) {  
            size_t insert_pos = (first_tombstone != (size_t)(-1)) ? first_tombstone : pos;

            tar->priv_used[insert_pos]   = HASH_FULL;
            tar->priv_keys[insert_pos]   = key;
            tar->priv_values[insert_pos] = value;
            tar->priv_size++;

            return SCC;
        }
    }

    // hashmap full -> cannot push (happens if rehash fails multiple times)
    return ERR;
}

#ifndef hashmap_push
    // Inserts new or replace value at given key
    // Given key and value are owned by the hashmap on success
    // May fail (if failed to resize), O(1) avg O(n) worst
    #define hashmap_push(inst) RIFF_INST(hashmap_push, inst)
#endif

int RIFF_INST(hashmap_find, INSTANCE)(hashmap(INSTANCE)* tar, KEY user_key, const KEY** inner_key, VAL** value) {
    if (tar->priv_capc == 0) return ERR; // empty map -> nothing can be found

    size_t idx = HASH(&user_key) % tar->priv_capc;
    for (size_t i = 0; i < tar->priv_capc; ++i) {
        size_t pos = (idx + i) % tar->priv_capc;

        // none -> no such key
        if (tar->priv_used[pos] == HASH_NONE)
            return ERR;

        // full -> check for equity -> return or continue
        if (tar->priv_used[pos] == HASH_FULL && EQUAL(&tar->priv_keys[pos], &user_key)) {
            if (inner_key) *inner_key = &tar->priv_keys[pos];
            if (value)     *value = &tar->priv_values[pos];
            return SCC;
        }
    }

    return ERR;
}

#ifndef hashmap_find
    // Searches hashmap for given user_key
    // If succeeded set *key to position of key (changes to it forbiden!)
    // and *value to position of value (can be changed)
    // Note changing the hasmap may lead to invalidation of returned values!
    // *key and *value may be NULL
    // May fail (if no given key), O(1) avg O(n) worst
    #define hashmap_find(inst) RIFF_INST(hashmap_find, inst)
#endif

void RIFF_INST(hashmap_pop, INSTANCE)(hashmap(INSTANCE)* tar, const KEY* INNER_key, VAL* out) {
    size_t pos = INNER_key - tar->priv_keys;

    if (out)  *out = tar->priv_values[pos];
    else VAL_DEST(&tar->priv_values[pos]);

    KEY_DEST(&tar->priv_keys[pos]);
    tar->priv_used[pos] = HASH_TOMB;
    tar->priv_size--;
}

#ifndef hashmap_pop
    // This function removes given key from the map
    //
    // IMPORTANT
    // INNER_key must be result of hashmap_find, (const KEY** inner_key)
    // called otherwise this function will lead to segfaults
    //
    // if out is NULL, stored value will be destructed (if destructor provided)
    // otherwise it will be moved into *out
    #define hashmap_pop(inst) RIFF_INST(hashmap_pop, inst)
#endif

void RIFF_INST(hashmap_clear, INSTANCE)(hashmap(INSTANCE)* tar) {
    for (size_t i = 0; i < tar->priv_capc; i++) {
        if (tar->priv_used[i] == HASH_FULL) {
            KEY_DEST(&tar->priv_keys[i]);
            VAL_DEST(&tar->priv_values[i]);
        }
        tar->priv_used[i] = HASH_NONE; // can do this
    }
    tar->priv_size = 0;
}

#ifndef hashmap_clear
    // Clears map
    // O(n)
    #define hashmap_clear(inst) RIFF_INST(hashmap_clear, inst)
#endif

#undef INSTANCE
#undef KEY
#undef KEY_DEST
#undef VAL
#undef VAL_DEST
#undef HASH
#undef EQUAL

#undef INIT_CAPC

#undef HASH_NONE
#undef HASH_FULL
#undef HASH_TOMB
