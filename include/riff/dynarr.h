/*
Mandatory macro params:
    SU - dynarr(SU) suffix
    T1 - stored type

Optional macro params:
    D1 - destructor for stored type                 (do not define if none)
    A  - custom allocator, same signature as malloc (defaults to malloc)
    R  - custom matching reallocator                (defaults to realloc)
    F  - custom matching free                       (defaults to free)
*/

/*
    Dynamic Array
    Data structure, dynamically resizing, 
    when it's size is to small to perform a push call
    O(n) memory complexity
*/

#include "generic.h"

/*
    Typedef
*/

#ifndef dynarr
    #define dynarr(LSU) NAME(dynarr, LSU)
#endif

typedef struct dynarr(SU) {
    size_t priv_size;
    size_t priv_capc;
    T1*    priv_data;
} dynarr(SU);

/*
    Helpers
*/

#ifdef D1
    #define D_LOOP(beg, end) for (T1* ptr = beg; ptr < end; ptr++) D1(ptr);
    #define D_CALL(ptr) D1(ptr)
#else
    #define D_LOOP(beg, end) ((void)0)
    #define D_CALL(ptr) ((void)0)
#endif

/*
    Zero / Destruction
*/

void IMPL(dynarr_zero, SU)(dynarr(SU)* tar) {
    tar->priv_size = 0;
    tar->priv_capc = 0;
    tar->priv_data = 0;
}

#ifndef dynarr_zero
    // Makes unitialized memory proper 0-initialized empty dynamic array
    // Does not free anything
    #define dynarr_zero(LSU) IMPL(dynarr_zero, LSU)
#endif

void IMPL(dynarr_destroy, SU)(dynarr(SU)* tar) {
    D_LOOP(tar->priv_data, tar->priv_data + tar->priv_size);
    F(tar->priv_data);
    dynarr_zero(SU)(tar);
}

#ifndef dynarr_destroy
    // Properly destroys given dynamic array
    // O(n) if destructor definied, O(1) otherwise
    #define dynarr_destroy(LSU) IMPL(dynarr_destroy, LSU)
#endif

/*
    Memory
*/

int IMPL(dynarr_reserve, SU)(dynarr(SU)* arr, size_t capacity) {
    if (arr->priv_capc >= capacity) return SCC; // already have
    
    // realloc into bigger block
    T1* new_data = (T1*)R(arr->priv_data, capacity * sizeof(T1));
    if (!new_data) return ERR; // realloc failed

    arr->priv_data = new_data;
    arr->priv_capc = capacity;
    return SCC;
}

#ifndef dynarr_reserve
    // Ensures dynamic array have at least given capacity (in total, not left)
    // May fail, O(1) else realocation time complexity
    #define dynarr_reserve(LSU) IMPL(dynarr_reserve, LSU)
#endif

int IMPL(dynarr_shrink_to_fit, SU)(dynarr(SU)* arr) {
    size_t new_cap = arr->priv_size;
    if (arr->priv_capc == new_cap) return SCC; // already shrunk

    // fall to zero state
    if (new_cap == 0) {
        dynarr_destroy(SU)(arr);
        return SCC;
    }
    
    // realloc block
    T1* new_data = R(arr->priv_data, new_cap * sizeof(T1));
    if (!new_data) return ERR; // realocation failed

    arr->priv_data = new_data;
    arr->priv_capc = new_cap;
    return SCC;
}

#ifndef dynarr_shrink_to_fit
    // Realoc array's memory into a block which tightly fit (to a minimum of 1 object)
    // arrays elements. If already shrunk, no effect.
    // May fail, O(1) else realocation time compelxity
    #define dynarr_shrink_to_fit(LSU) IMPL(dynarr_shrink_to_fit, LSU)
#endif

/*
    Getters
*/

size_t IMPL(dynarr_size, SU)(dynarr(SU)* arr) {
    return arr->priv_size;
}

#ifndef dynarr_size
    // Returns count of elements in the dynamic array
    #define dynarr_size(LSU) IMPL(dynarr_size, LSU)
#endif

size_t IMPL(dynarr_capacity, SU)(dynarr(SU)* arr) {
    return arr->priv_capc;
}

#ifndef dynarr_capacity
    // Proper way of obtaining count of elements in dynamic array
    #define dynarr_capacity(LSU) IMPL(dynarr_capacity, LSU)
#endif

/*
    Access
*/

T1* IMPL(dynarr_access, SU)(dynarr(SU)* arr) {
    return (T1*)arr->priv_data;
}

#ifndef dynarr_access
    // Returns pointer to first element of contiguous block of memory, the array
    // Only the first dynarr_size() amount of elements are initialized and valid
    // You can change given memory, but must keep object valid, as destructor (if provided)
    // will be called on them sooner or later
    // O(1)
    #define dynarr_access(LSU) IMPL(dynarr_access, LSU)
#endif

const T1* IMPL(dynarr_const_access, SU)(const dynarr(SU)* arr) {
    return (const T1*)arr->priv_data;
}

#ifndef dynarr_const_access
    // Returns pointer to first element of contiguous block of memory, the array
    // Only the first dynarr_size() amount of elements are initialized and valid
    // O(1)
    #define dynarr_const_access(LSU) IMPL(dynarr_const_access, LSU)
#endif

/*
    Operations
*/

int IMPL(dynarr_push, SU)(dynarr(SU)* arr, T1 value) {
    if (arr->priv_size >= arr->priv_capc) {
        size_t new_cap = arr->priv_capc ? arr->priv_capc * 2 : 1;
        void* new_data = (T1*)R(arr->priv_data, new_cap * sizeof(T1));
        if (!new_data) return ERR; // allocation failed
        arr->priv_capc = new_cap;
        arr->priv_data = new_data;
    }
    ((T1*)arr->priv_data)[arr->priv_size++] = value;
    return SCC;
}

#ifndef dynarr_push
    // This operation push new element into dynamic array
    // May cause reallocation of dynamic array memory - watch out for your pointers
    // May fail, O(1) average
    #define dynarr_push(LSU) IMPL(dynarr_push, LSU)
#endif

int IMPL(dynarr_pop, SU)(dynarr(SU)* arr, T1* out) {
    if (arr->priv_size == 0) return ERR; // nothing to pop
    arr->priv_size--;

    // transfer to out
    if (out) *out = ((T1*)arr->priv_data)[arr->priv_size];
    // destroy element
    else D_CALL(&((T1*)arr->priv_data)[arr->priv_size]);

    return SCC;
}

#ifndef dynarr_pop
    // Pops last element of the dynamic array
    // If out is null destructor (if provided) will be called on the poped object
    // Else object will be moved into *out
    // May fail (nothing to pop), O(1)
    #define dynarr_pop(LSU) IMPL(dynarr_pop, LSU)
#endif

int IMPL(dynarr_pop_many, SU)(dynarr(SU)* arr, T1* out, size_t amount) {
    if (arr->priv_size < amount) return ERR; // not enough elements

    size_t first = arr->priv_size - amount;

    if (out) for (size_t i = 0; i < amount; i++) out[i] = arr->priv_data[first + i];
    else  D_LOOP(arr->priv_data + first, arr->priv_data + arr->priv_size);

    arr->priv_size = first;
    return SCC;
}

#ifndef dynarr_pop_many
    // Pops out block of dynamic array elements, not only the last one
    // If out == NULL destructor is called on objects
    // Otherwise objects are copied into *out as described:
    // Object orders is preserved, eg. if dynamic array is {a, b, c, d}
    // calling dynarr_pop_many(arr, out, 3) would make array {a} and out {b, c, d}
    // Obviously out must be large enough array to store the elements
    // May fail (not enough elements to pop), O(amount)
    #define dynarr_pop_many(LSU) IMPL(dynarr_pop_many, LSU)
#endif

void IMPL(dynarr_clear, SU)(dynarr(SU)* arr) {
    D_LOOP(arr->priv_data, arr->priv_data + arr->priv_size);
    arr->priv_size = 0;
}

#ifndef dynarr_clear
    // Clear array, erases contained elements with destructor if provided, 
    // but does not reduce it capacity - combine with dynarr_shrink_to_fit() 
    // call afterwards to reduce array memory block
    #define dynarr_clear(LSU) IMPL(dynarr_clear, LSU)
#endif

#undef D_CALL
#undef D_LOOP
