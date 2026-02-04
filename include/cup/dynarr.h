/*
Mandatory macro params:
    SU - name suffix
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

#define NAME NAME_MAKE(dynarr, SU)

#ifdef D1
    #define D_LOOP(beg, end) for (T1* ptr = beg; ptr < end; ptr++) D1(ptr);
    #define D_CALL(ptr) D1(ptr)
#else
    #define D_LOOP(beg, end) ((void)0)
    #define D_CALL(ptr) ((void)0)
#endif

typedef struct NAME {
    size_t priv_size;
    size_t priv_capc;
    T1*    priv_data;
} NAME;

/*
    Initialization / Destruction
*/

int FUNC_IMPL(dynarr_init)(NAME* tar, size_t initial_capacity) {
    // do not allow capacity == 0, so we dont have to check on that later
    if (initial_capacity == 0) initial_capacity = 1;
    tar->priv_size = 0;
    tar->priv_data = (T1*)A(initial_capacity * sizeof(T1));
    if (!tar->priv_data) return ERR; // allocation error
    tar->priv_capc = initial_capacity;
    return SCC;
}

#ifndef dynarr_init
    // Inits dynamic array with given capacity
    // May fail, O(1)
    #define dynarr_init(LSU) FUNC_RESP(dynarr_init, LSU)
#endif

void FUNC_IMPL(dynarr_destroy)(NAME* tar) {
    D_LOOP(tar->priv_data, tar->priv_data + tar->priv_size);
    F(tar->priv_data);
    tar->priv_data = NULL;
    tar->priv_capc = 0;
    tar->priv_size = 0;
}

#ifndef dynarr_destroy
    // Properly destroys given dynamic array
    // O(n) if destructor definied, O(1) otherwise
    #define dynarr_destroy(LSU) FUNC_RESP(dynarr_destroy, LSU)
#endif

/*
    Memory
*/

int FUNC_IMPL(dynarr_reserve)(NAME* arr, size_t capacity) {
    if (arr->priv_capc >= capacity) return SCC; // already have
    
    T1* new_data = (T1*)R(arr->priv_data, capacity * sizeof(T1));
    if (!new_data) return ERR; // realloc failed

    arr->priv_data = new_data;
    arr->priv_capc = capacity;
    return SCC;
}

#ifndef dynarr_reserve
    // Ensures dynamic array have at least given capacity (in total, not left)
    // May fail, O(1) else realocation time complexity
    #define dynarr_reserve(LSU) FUNC_RESP(dynarr_reserve, LSU)
#endif

int FUNC_IMPL(dynarr_shrink_to_fit)(NAME* arr) {
    size_t new_cap = arr->priv_size;
    if (new_cap == 0) new_cap++; // never cause data to be NULL
    if (arr->priv_capc == new_cap) return SCC; // already shrunk
    
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
    #define dynarr_shrink_to_fit(LSU) FUNC_RESP(dynarr_shrink_to_fit, LSU)
#endif

/*
    Getters
*/

size_t FUNC_IMPL(dynarr_size)(NAME* arr) {
    return arr->priv_size;
}

#ifndef dynarr_size
    // Returns count of elements in the dynamic array
    #define dynarr_size(LSU) FUNC_RESP(dynarr_size, LSU)
#endif

size_t FUNC_IMPL(dynarr_capacity)(NAME* arr) {
    return arr->priv_capc;
}

#ifndef dynarr_capacity
    // Proper way of obtaining count of elements in dynamic array
    #define dynarr_capacity(LSU) FUNC_RESP(dynarr_capacity, LSU)
#endif

/*
    Access
*/

T1* FUNC_IMPL(dynarr_access)(NAME* arr) {
    return (T1*)arr->priv_data;
}

#ifndef dynarr_access
    // Returns pointer to first element of contiguous block of memory, the array
    // Only the first dynarr_size() amount of elements are initialized and valid
    // You can change given memory, but must keep object valid, as destructor (if provided)
    // will be called on them sooner or later
    // O(1)
    #define dynarr_access(LSU) FUNC_RESP(dynarr_access, LSU)
#endif

const T1* FUNC_IMPL(dynarr_const_access)(const NAME* arr) {
    return (const T1*)arr->priv_data;
}

#ifndef dynarr_const_access
    // Returns pointer to first element of contiguous block of memory, the array
    // Only the first dynarr_size() amount of elements are initialized and valid
    // O(1)
    #define dynarr_const_access(LSU) FUNC_RESP(dynarr_const_access, LSU)
#endif

/*
    Operations
*/

int FUNC_IMPL(dynarr_push)(NAME* arr, T1 value) {
    if (arr->priv_size >= arr->priv_capc) {
        size_t new_cap = arr->priv_capc * 2;
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
    #define dynarr_push(LSU) FUNC_RESP(dynarr_push, LSU)
#endif

int FUNC_IMPL(dynarr_pop)(NAME* arr, T1* out) {
    if (arr->priv_size == 0) return ERR; // nothing to pop
    arr->priv_size--;

    // transfer to out
    if (out) *out = ((T1*)arr->priv_data)[arr->priv_size];
    // destroy element
    else     D_CALL(&((T1*)arr->priv_data)[arr->priv_size]);

    return SCC;
}

#ifndef dynarr_pop
    // Pops last element of the dynamic array
    // If out is null destructor (if provided) will be called on the poped object
    // Else object will be moved into *out
    // May fail (nothing to pop), O(1)
    #define dynarr_pop(LSU) FUNC_RESP(dynarr_pop, LSU)
#endif

int FUNC_IMPL(dynarr_pop_many)(NAME* arr, T1* out, size_t amount) {
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
    #define dynarr_pop_many(LSU) FUNC_RESP(dynarr_pop_many, LSU)
#endif

void FUNC_IMPL(dynarr_clear)(NAME* arr) {
    D_LOOP(arr->priv_data, arr->priv_data + arr->priv_size);
    arr->priv_size = 0;
}

#ifndef dynarr_clear
    // Clear array, erases contained elements with destructor if provided, 
    // but does not reduce it capacity - combine with dynarr_shrink_to_fit() 
    // call afterwards to reduce array memory block
    #define dynarr_clear(LSU) FUNC_RESP(dynarr_clear, LSU)
#endif

#undef D_CALL
#undef D_LOOP
#undef NAME
