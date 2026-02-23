/*
    T macro pattern
        [instance name], [stored type], [stored type destructor (opt)]
*/

#include "generic.h"

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

#ifndef darr
    /*
        Dynamic Array (darr)
        Data structure, dynamically resizing, 
        when it's size is to small to perform a push call
        O(n) memory complexity
    */
    #define darr(inst) RIFF_NAME(darr, inst)
#endif

typedef struct darr(INSTANCE) {
    size_t  priv_size;
    size_t  priv_capc;
    STORED* priv_data;
} darr(INSTANCE);

/*
    Zero / Destruction
*/

void RIFF_INST(darr_zero, INSTANCE)(darr(INSTANCE)* tar) {
    tar->priv_size = 0;
    tar->priv_capc = 0;
    tar->priv_data = 0;
}

#ifndef darr_zero
    // Makes unitialized memory proper 0-initialized empty dynamic array
    // Does not free anything
    #define darr_zero(inst) RIFF_INST(darr_zero, inst)
#endif

void RIFF_INST(darr_destroy, INSTANCE)(darr(INSTANCE)* tar) {
    DESTRUCTOR_LOOP(tar->priv_data, tar->priv_data + tar->priv_size);
    RIFF_FREE(tar->priv_data);
    darr_zero(INSTANCE)(tar);
}

#ifndef darr_destroy
    // Properly destroys given dynamic array
    // O(n) if destructor definied, O(1) otherwise
    #define darr_destroy(inst) RIFF_INST(darr_destroy, inst)
#endif

/*
    Memory
*/

int RIFF_INST(darr_reserve, INSTANCE)(darr(INSTANCE)* arr, size_t capacity) {
    if (arr->priv_capc >= capacity) return SCC; // already have
    
    // realloc into bigger block
    STORED* new_data = (STORED*)RIFF_REALLOC(arr->priv_data, capacity * sizeof(STORED));
    if (!new_data) return ERR; // realloc failed

    arr->priv_data = new_data;
    arr->priv_capc = capacity;
    return SCC;
}

#ifndef darr_reserve
    // Ensures dynamic array have at least given capacity (in total, not left)
    // May fail, O(1) else realocation time complexity
    #define darr_reserve(inst) RIFF_INST(darr_reserve, inst)
#endif

int RIFF_INST(darr_shrink_to_fit, INSTANCE)(darr(INSTANCE)* arr) {
    size_t new_cap = arr->priv_size;
    if (arr->priv_capc == new_cap) return SCC; // already shrunk

    // fall to zero state
    if (new_cap == 0) {
        darr_destroy(INSTANCE)(arr);
        return SCC;
    }
    
    // realloc block
    STORED* new_data = RIFF_REALLOC(arr->priv_data, new_cap * sizeof(STORED));
    if (!new_data) return ERR; // realocation failed

    arr->priv_data = new_data;
    arr->priv_capc = new_cap;
    return SCC;
}

#ifndef darr_shrink_to_fit
    // Realoc array's memory into a block which tightly fit (to a minimum of 1 object)
    // arrays elements. If already shrunk, no effect.
    // May fail, O(1) else realocation time compelxity
    #define darr_shrink_to_fit(inst) RIFF_INST(darr_shrink_to_fit, inst)
#endif

/*
    Getters
*/

size_t RIFF_INST(darr_size, INSTANCE)(darr(INSTANCE)* arr) {
    return arr->priv_size;
}

#ifndef darr_size
    // Returns count of elements in the dynamic array
    #define darr_size(inst) RIFF_INST(darr_size, inst)
#endif

size_t RIFF_INST(darr_capacity, INSTANCE)(darr(INSTANCE)* arr) {
    return arr->priv_capc;
}

#ifndef darr_capacity
    // Proper way of obtaining count of elements in dynamic array
    #define darr_capacity(inst) RIFF_INST(darr_capacity, inst)
#endif

/*
    Access
*/

STORED* RIFF_INST(darr_access, INSTANCE)(darr(INSTANCE)* arr) {
    return (STORED*)arr->priv_data;
}

#ifndef darr_access
    // Returns pointer to first element of contiguous block of memory, the array
    // Only the first darr_size() amount of elements are initialized and valid
    // You can change given memory, but must keep object valid, as destructor (if provided)
    // will be called on them sooner or later
    // O(1)
    #define darr_access(inst) RIFF_INST(darr_access, inst)
#endif

const STORED* RIFF_INST(darr_const_access, INSTANCE)(const darr(INSTANCE)* arr) {
    return (const STORED*)arr->priv_data;
}

#ifndef darr_const_access
    // Returns pointer to first element of contiguous block of memory, the array
    // Only the first darr_size() amount of elements are initialized and valid
    // O(1)
    #define darr_const_access(inst) RIFF_INST(darr_const_access, inst)
#endif

/*
    Operations
*/

int RIFF_INST(darr_push, INSTANCE)(darr(INSTANCE)* arr, STORED value) {
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

#ifndef darr_push
    // This operation push new element into dynamic array
    // If succeeded dynamic array is now the owner of the object
    // May cause reallocation of dynamic array memory - watch out for your pointers
    // May fail, O(1) average
    #define darr_push(inst) RIFF_INST(darr_push, inst)
#endif

int RIFF_INST(darr_pop, INSTANCE)(darr(INSTANCE)* arr, STORED* out) {
    if (arr->priv_size == 0) return ERR; // nothing to pop
    arr->priv_size--;

    // transfer to out
    if (out) *out = ((STORED*)arr->priv_data)[arr->priv_size];
    // destroy element
    else DESTRUCTOR(&((STORED*)arr->priv_data)[arr->priv_size]);

    return SCC;
}

#ifndef darr_pop
    // Pops last element of the dynamic array
    // If out is null destructor (if provided) will be called on the poped object
    // Else object will be moved into *out
    // May fail (nothing to pop), O(1)
    #define darr_pop(inst) RIFF_INST(darr_pop, inst)
#endif

int RIFF_INST(darr_pop_many, INSTANCE)(darr(INSTANCE)* arr, STORED* out, size_t amount) {
    if (arr->priv_size < amount) return ERR; // not enough elements

    size_t first = arr->priv_size - amount;

    if (out) for (size_t i = 0; i < amount; i++) out[i] = arr->priv_data[first + i];
    else  DESTRUCTOR_LOOP(arr->priv_data + first, arr->priv_data + arr->priv_size);

    arr->priv_size = first;
    return SCC;
}

#ifndef darr_pop_many
    // Pops out block of dynamic array elements, not only the last one
    // If out == NULL destructor is called on objects
    // Otherwise objects are copied into *out as described:
    // Object orders is preserved, eg. if dynamic array is {a, b, c, d}
    // calling darr_pop_many(arr, out, 3) would make array {a} and out {b, c, d}
    // Obviously out must be large enough array to store the elements
    // May fail (not enough elements to pop), O(amount)
    #define darr_pop_many(inst) RIFF_INST(darr_pop_many, inst)
#endif

void RIFF_INST(darr_clear, INSTANCE)(darr(INSTANCE)* arr) {
    DESTRUCTOR_LOOP(arr->priv_data, arr->priv_data + arr->priv_size);
    arr->priv_size = 0;
}

#ifndef darr_clear
    // Clear array, erases contained elements with destructor if provided, 
    // but does not reduce it capacity - combine with darr_shrink_to_fit() 
    // call afterwards to reduce array memory block
    #define darr_clear(inst) RIFF_INST(darr_clear, inst)
#endif

#undef DESTRUCTOR_LOOP

#undef INSTANCE
#undef STORED
#undef DESTRUCTOR
