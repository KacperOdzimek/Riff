/*
Mandatory macro params:
    SU - name  suffix
    T1 - key   type
    T2 - value type
    F1 - key type hash  function - size_t(func)(const T1*)
    F2 - key type equal function - int(func)(const T1* a, const T1* b) (non-0 if equal)

Optional macro params:
    D1 - destructor for key type   (do not define if none)
    D2 - destructor for value type (do not define if none)

    A  - custom allocator, same signature as malloc (defaults to malloc)
    R  - custom matching reallocator                (defaults to realloc)
    F  - custom matching free                       (defaults to free)
*/

/*
    Hash Map
*/

#include "generic.h"

#define NAME NAME_MAKE(hashmap, SU)

#ifdef D1
#define KEY_D_CALL(ptr) D1(ptr)
#else
#define KEY_D_CALL(ptr) ((void)0)
#endif

#ifdef D2
#define VAL_D_CALL(ptr) D2(ptr)
#else
#define VAL_D_CALL(ptr) ((void)0)
#endif

#define HASH_NONE 0
#define HASH_FULL 1
#define HASH_TOMB 2

typedef struct NAME
{
    char*  priv_used;
    T1 *   priv_keys;
    T2 *   priv_values;
    size_t priv_size; // actual count of items within
    size_t priv_capc; // size of arrays
} NAME;

/*
    Initialization / Destruction
*/

int FUNC_INTR(hashmap_init, SU)(NAME* tar, size_t cap) {
    tar->priv_size = 0;
    tar->priv_capc = cap;

    tar->priv_used = (char *)A(tar->priv_capc * sizeof(char));
    if (!tar->priv_used) return ERR;

    tar->priv_keys = (T1 *)A(tar->priv_capc * sizeof(T1));
    if (!tar->priv_keys) { F(tar->priv_used); return ERR; }

    tar->priv_values = (T2 *)A(tar->priv_capc * sizeof(T2));
    if (!tar->priv_values) { F(tar->priv_used); F(tar->priv_keys); return ERR; }

    for (size_t i = 0; i < tar->priv_capc; ++i) tar->priv_used[i] = HASH_NONE;
    return SCC;
}

int FUNC_IMPL(hashmap_init, SU)(NAME *tar) {
    return FUNC_INTR(hashmap_init, SU)(tar, 16);
}

#ifndef hashmap_init
    // Inits hashmap structure
    // May fail
    #define hashmap_init(LSU) FUNC_IMPL(hashmap_init, LSU)
#endif

void FUNC_IMPL(hashmap_destroy, SU)(NAME *tar) {
    for (size_t i = 0; i < tar->priv_capc; i++) {
        if (tar->priv_used[i]) {
            KEY_D_CALL(&tar->priv_keys[i]);
            VAL_D_CALL(&tar->priv_values[i]);
        }
    }

    if (tar->priv_used)   F(tar->priv_used);
    if (tar->priv_keys)   F(tar->priv_keys);
    if (tar->priv_values) F(tar->priv_values);

    tar->priv_used   = NULL;
    tar->priv_keys   = NULL;
    tar->priv_values = NULL;
    tar->priv_size   = 0;
    tar->priv_capc   = 0;
}

#ifndef hashmap_destroy
    // Frees hashmap and its keys and values
    // O(n)
    #define hashmap_destroy(LSU) FUNC_IMPL(hashmap_destroy, LSU)
#endif

/*
    Memory
*/

int FUNC_IMPL(hashmap_push, SU)(NAME *tar, T1 key, T2 value); // forward

int FUNC_IMPL(hashmap_rehash, SU)(NAME *tar, size_t new_capacity) {
    // alloc new map
    if (new_capacity < tar->priv_size) return ERR;
    NAME new_map; if (FUNC_INTR(hashmap_init, SU)(&new_map, new_capacity) == ERR) return ERR;

    // reinsert items into new map
    for (size_t i = 0; i < tar->priv_capc; ++i) {
        if (tar->priv_used[i] != HASH_FULL) continue;
        int scc = FUNC_IMPL(hashmap_push, SU)(&new_map, tar->priv_keys[i], tar->priv_values[i]);
        
        // if failed to insert, destroy partialy created nao
        if (scc != SCC) {
            FUNC_IMPL(hashmap_destroy, SU)(&new_map);
            return ERR;
        }
    }

    // delete old arrays
    // do not use hashmap_destroy not to call destructors
    F(tar->priv_used);
    F(tar->priv_keys);
    F(tar->priv_values);

    // if everything succeded move new map into old map
    *tar = new_map;
    return SCC;
}

#ifndef hashmap_rehash
    // Rebuild iternal arrays inside hashmap
    // May fail (new_capacity to small to fit, or allocation failure), O(n)
    #define hashmap_rehash(LSU) FUNC_IMPL(hashmap_rehash, LSU)
#endif

/*
    Operations
*/

int FUNC_IMPL(hashmap_find, SU)(NAME* tar, T1 user_key, const T1** inner_key, T2** value) {
    size_t idx = F1(&user_key) % tar->priv_capc;

    for (size_t i = 0; i < tar->priv_capc; ++i) {
        size_t pos = (idx + i) % tar->priv_capc;

        // none -> no such key
        if (tar->priv_used[pos] == HASH_NONE)
            return ERR;

        // full -> check for equity -> return or continue
        if (tar->priv_used[pos] == HASH_FULL && F2(&tar->priv_keys[pos], &user_key)) {
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
    #define hashmap_find(LSU) FUNC_IMPL(hashmap_find, LSU)
#endif

void FUNC_IMPL(hashmap_pop, SU)(NAME* tar, const T1* INNER_key, T2* out) {
    size_t pos = INNER_key - tar->priv_keys;

    if (out)  *out = tar->priv_values[pos];
    else VAL_D_CALL(&tar->priv_values[pos]);

    KEY_D_CALL(&tar->priv_keys[pos]);
    tar->priv_used[pos] = HASH_TOMB;
    tar->priv_size--;
}

#ifndef hashmap_pop
    // This function removes given key from the map
    //
    // IMPORTANT
    // INNER_key must be result of hashmap_finc, (const T1** inner_key)
    // called otherwise this function will lead to segfaults
    //
    // if out is NULL, stored value will be destructed (if destructor provided)
    // otherwise it will be moved into *out
    #define hashmap_pop(LSU) FUNC_IMPL(hashmap_pop, LSU)
#endif

int FUNC_IMPL(hashmap_push, SU)(NAME* tar, T1 key, T2 value) {
    // Double memory if load factor exceeds 0.7
    if ((tar->priv_size + 1) * 10 > tar->priv_capc * 7) {
        // grow, if grow fails try to fit anyway - there still may be some free spots in the array
        FUNC_IMPL(hashmap_rehash, SU)(tar, tar->priv_capc * 2);
    }

    size_t idx  = F1(&key) % tar->priv_capc;
    size_t first_tombstone = (size_t)(-1);

    for (size_t i = 0; i < tar->priv_capc; ++i) {
        size_t pos = (idx + i) % tar->priv_capc;

         // check if key is exactly the same, if so replace value
        if (tar->priv_used[pos] == HASH_FULL && F2(&tar->priv_keys[pos], &key)) {
            VAL_D_CALL(&tar->priv_values[pos]); // free old value
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
    // Given key and value are now managed my map and provided deconstructors
    // May fail (if failed to resize), O(1) avg O(n) worst
    #define hashmap_push(LSU) FUNC_IMPL(hashmap_push, LSU)
#endif

void FUNC_IMPL(hashmap_clear, SU)(NAME* tar) {
    for (size_t i = 0; i < tar->priv_capc; i++) {
        if (tar->priv_used[i]) {
            KEY_D_CALL(&tar->priv_keys[i]);
            VAL_D_CALL(&tar->priv_values[i]);
        }
        tar->priv_used[i] = HASH_NONE; // can do this
    }
}

#ifndef hashmap_clear
    // Clears map
    // O(n)
    #define hashmap_clear(LSU) FUNC_IMPL(hashmap_clear, LSU)
#endif

#undef HASH_NONE
#undef HASH_FULL
#undef HASH_TOMB

#undef KEY_D_CALL
#undef VAL_D_CALL
#undef NAME
