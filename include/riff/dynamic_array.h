/*
    T macro pattern
        [instance name], [stored type], [stored type destructor (opt)]
*/

#include "generic.h"

#ifndef T
    #error No "T" macro defined at the time of inclusion. Note T macros are undef at the end of every data structure header.
#endif

#ifndef A
    #error No "A" macro defined at the time of inclusion. Note A macros are undef at the end of every data structure header.
#endif

/*
    Unpack and Helpers
*/

#define INSTANCE   RIFF_FIRST(T)
#define STORED     RIFF_SECOND(T)
#define DESTRUCTOR RIFF_THIRD(T)

#define DESTRUCTOR_LOOP(beg, end) \
    for (STORED* ptr = beg; ptr < end; ptr++) DESTRUCTOR(ptr);

/*
    Typedef
*/

// Dynamic Array (dyarr)
// Data structure, dynamically resizing, 
// when it's size is too small to perform a push call
// O(n) memory complexity
#define dyarr(inst) RIFF_INST(dyarr, inst)

typedef struct dyarr(INSTANCE) {
    size_t  priv_size;
    size_t  priv_capc;
    STORED* priv_data;
} dyarr(INSTANCE);

/*
    Zero / Destruction
*/

// Makes unitialized memory proper 0-initialized empty dynamic array
// Does not free anything
#define dyarr_zero(inst) RIFF_INST(dyarr_zero, inst)

RIFF_API(void) dyarr_zero(INSTANCE)(dyarr(INSTANCE)* tar) {
    tar->priv_size = 0;
    tar->priv_capc = 0;
    tar->priv_data = 0;
}

// Properly destroys given dynamic array
// O(n) if destructor definied, O(1) otherwise
#define dyarr_destroy(inst) RIFF_INST(dyarr_destroy, inst)

RIFF_API(void) dyarr_destroy(INSTANCE)(dyarr(INSTANCE)* tar) {
    DESTRUCTOR_LOOP(tar->priv_data, tar->priv_data + tar->priv_size);
    RIFF_FREE(tar->priv_data);
    dyarr_zero(INSTANCE)(tar);
}

/*
    Memory
*/

// Ensures dynamic array have at least given capacity (in total, not left)
// May fail, O(1) else reallocation time complexity
#define dyarr_reserve(inst) RIFF_INST(dyarr_reserve, inst)

RIFF_API(int) dyarr_reserve(INSTANCE)(dyarr(INSTANCE)* arr, size_t capacity) {
    if (arr->priv_capc >= capacity) return SCC; // already have
    
    // realloc into bigger block
    STORED* new_data = (STORED*)RIFF_REALLOC(arr->priv_data, capacity * sizeof(STORED));
    if (!new_data) return ERR; // realloc failed

    arr->priv_data = new_data;
    arr->priv_capc = capacity;
    return SCC;
}

// Realoc array's memory into a block which tightly fit arrays elements. 
// If already shrunk, no effect.
// May fail, O(1) else reallocation time complexity
#define dyarr_shrink_to_fit(inst) RIFF_INST(dyarr_shrink_to_fit, inst)

RIFF_API(int) dyarr_shrink_to_fit(INSTANCE)(dyarr(INSTANCE)* arr) {
    size_t new_cap = arr->priv_size;
    if (arr->priv_capc == new_cap) return SCC; // already shrunk

    // fall to zero state
    if (new_cap == 0) {
        dyarr_destroy(INSTANCE)(arr);
        return SCC;
    }
    
    // realloc block
    STORED* new_data = RIFF_REALLOC(arr->priv_data, new_cap * sizeof(STORED));
    if (!new_data) return ERR; // reallocation failed

    arr->priv_data = new_data;
    arr->priv_capc = new_cap;
    return SCC;
}

/*
    Query
*/

// Returns count of elements in the dynamic array
#define dyarr_size(inst) RIFF_INST(dyarr_size, inst)

RIFF_API(size_t) dyarr_size(INSTANCE)(dyarr(INSTANCE)* arr) {
    return arr->priv_size;
}

// Proper way of obtaining count of elements in dynamic array
#define dyarr_capacity(inst) RIFF_INST(dyarr_capacity, inst)

RIFF_API(size_t) dyarr_capacity(INSTANCE)(dyarr(INSTANCE)* arr) {
    return arr->priv_capc;
}

/*
    Access
*/

// Returns pointer to first element of contiguous block of memory, the array
// Only the first dyarr_size() amount of elements are initialized and valid
// You can change given memory, but must keep object valid, as destructor (if provided)
// will be called on them sooner or later
// O(1)
#define dyarr_access(inst) RIFF_INST(dyarr_access, inst)

RIFF_API(STORED*) dyarr_access(INSTANCE)(dyarr(INSTANCE)* arr) {
    return (STORED*)arr->priv_data;
}

// Returns pointer to first element of contiguous block of memory, the array
// Only the first dyarr_size() amount of elements are initialized and valid
// O(1)
#define dyarr_const_access(inst) RIFF_INST(dyarr_const_access, inst)

RIFF_API(const STORED*) dyarr_const_access(INSTANCE)(const dyarr(INSTANCE)* arr) {
    return (const STORED*)arr->priv_data;
}

/*
    Operations
*/

// This operation push new element into dynamic array
// If succeeded dynamic array is now the owner of the object
// May cause reallocation of dynamic array memory - watch out for your pointers
// May fail, O(1) average
#define dyarr_push(inst) RIFF_INST(dyarr_push, inst)

RIFF_API(int) dyarr_push(INSTANCE)(dyarr(INSTANCE)* arr, STORED value) {
    if (arr->priv_size >= arr->priv_capc) {
        size_t new_cap = arr->priv_capc ? arr->priv_capc * 2 : 1;
        void* new_data = (STORED*)RIFF_REALLOC(arr->priv_data, new_cap * sizeof(STORED));
        if (!new_data) return ERR; // allocation failed
        arr->priv_capc = new_cap;
        arr->priv_data = new_data;
    }
    ((STORED*)arr->priv_data)[arr->priv_size++] = value;
    return SCC;
}

// Pops last element of the dynamic array
// If out is null destructor (if provided) will be called on the poped object
// Else object will be moved into *out
// May fail (nothing to pop), O(1)
#define dyarr_pop(inst) RIFF_INST(dyarr_pop, inst)

RIFF_API(int) dyarr_pop(INSTANCE)(dyarr(INSTANCE)* arr, STORED* out) {
    if (arr->priv_size == 0) return ERR; // nothing to pop
    arr->priv_size--;

    // transfer to out
    if (out) *out = ((STORED*)arr->priv_data)[arr->priv_size];
    // destroy element
    else DESTRUCTOR(&((STORED*)arr->priv_data)[arr->priv_size]);

    return SCC;
}

// Pops out block of dynamic array elements, not only the last one
// If out == NULL destructor is called on objects
// Otherwise objects are copied into *out as described:
// Object orders is preserved, eg. if dynamic array is {a, b, c, d}
// calling dyarr_pop_many(arr, out, 3) would make array {a} and out {b, c, d}
// Obviously out must be large enough array to store the elements
// May fail (not enough elements to pop), O(amount)
#define dyarr_pop_many(inst) RIFF_INST(dyarr_pop_many, inst)

RIFF_API(int) dyarr_pop_many(INSTANCE)(dyarr(INSTANCE)* arr, STORED* out, size_t amount) {
    if (arr->priv_size < amount) return ERR; // not enough elements

    size_t first = arr->priv_size - amount;

    if (out) for (size_t i = 0; i < amount; i++) out[i] = arr->priv_data[first + i];
    else  DESTRUCTOR_LOOP(arr->priv_data + first, arr->priv_data + arr->priv_size);

    arr->priv_size = first;
    return SCC;
}

// Clear array, erases contained elements with destructor if provided, 
// but does not reduce it capacity - combine with dyarr_shrink_to_fit() 
// call afterwards to reduce array memory block
#define dyarr_clear(inst) RIFF_INST(dyarr_clear, inst)

RIFF_API(void) dyarr_clear(INSTANCE)(dyarr(INSTANCE)* arr) {
    DESTRUCTOR_LOOP(arr->priv_data, arr->priv_data + arr->priv_size);
    arr->priv_size = 0;
}

#undef DESTRUCTOR_LOOP

#undef INSTANCE
#undef STORED
#undef DESTRUCTOR

// consume parameters
#undef T
#undef A
