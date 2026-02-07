#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

size_t str_hash(const char** s) {
    size_t hash = 5381;
    const char* p = *s;
    while (*p) hash = ((hash << 5) + hash) + (unsigned char)*p++;
    return hash;
}

int str_eq(const char** a, const char** b) {
    return strcmp(*a, *b) == 0;
}

void dec(const char** c) {
    free((void*)*c);
}

#define SU txt_int
#define T1 const char*
#define D1 dec
#define T2 int
#define F1 str_hash
#define F2 str_eq
#include "cup/hashmap.h"

int main(void) {
    hashmap_txt_int map;
    assert(hashmap_init(txt_int)(&map) == SCC);

    // ---------------- Insert ----------------
    hashmap_push(txt_int)(&map, strdup("apple"), 10);
    hashmap_push(txt_int)(&map, strdup("banana"), 20);
    hashmap_push(txt_int)(&map, strdup("cherry"), 30);

    // ---------------- Find ----------------
    T2 *val;
    assert(hashmap_find(txt_int)(&map, "apple", NULL, &val) == SCC);
    assert(*val == 10);

    assert(hashmap_find(txt_int)(&map, "banana", NULL, &val) == SCC);
    assert(*val == 20);

    assert(hashmap_find(txt_int)(&map, "cherry", NULL, &val) == SCC);
    assert(*val == 30);

    // ---------------- Replace ----------------
    hashmap_push(txt_int)(&map, strdup("banana"), 25);
    assert(hashmap_find(txt_int)(&map, "banana", NULL, &val) == SCC);
    assert(*val == 25);

    // ---------------- Delete ----------------
    T1 *inner_key;
    assert(hashmap_find(txt_int)(&map, "apple", &inner_key, NULL) == SCC);
    hashmap_pop(txt_int)(&map, inner_key, val); // pop with value
    assert(*val == 10);

    assert(hashmap_find(txt_int)(&map, "apple", NULL, &val) == ERR); // apple removed

    // ---------------- Tombstone check ----------------
    hashmap_push(txt_int)(&map, strdup("apple"), 15); // reuse tombstone
    assert(hashmap_find(txt_int)(&map, "apple", NULL, &val) == SCC);
    assert(*val == 15);

    // ---------------- Rehash ----------------
    for (int i = 0; i < 100; i++) {
        char buf[16];
        snprintf(buf, sizeof(buf), "key%d", i);
        hashmap_push(txt_int)(&map, strdup(buf), i);
    }
    // Check a few keys after rehash
    assert(hashmap_find(txt_int)(&map, "key0", NULL, &val) == SCC);
    assert(*val == 0);
    assert(hashmap_find(txt_int)(&map, "key50", NULL, &val) == SCC);
    assert(*val == 50);
    assert(hashmap_find(txt_int)(&map, "key99", NULL, &val) == SCC);
    assert(*val == 99);

    // ---------------- Destroy ----------------
    hashmap_destroy(txt_int)(&map);

    printf("ALL TESTS PASSED\n");
    return 0;
}
