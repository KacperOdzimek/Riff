/*
Mandatory macro params:
    SU - name suffix
    T1 - stored type

Optional macro params:
    D1 - destructor for stored type                 (do not define if none)
    A  - custom allocator, same signature as malloc (optional, defaults to malloc)
*/

/*
    Dynamic Array
    Data structure, dynamically resizing, 
    when it's size is to small to perform a push call
    O(n) ~ O(2n) memory complexity
*/

#include "generic.h"

#define NAME NAME_MAKE(dynarr_, SU)

#ifdef D1
    #define D_LOOP(beg, end) for (T1* ptr = beg; ptr < end; ptr++) D1(ptr);
    #define D_CALL(ptr) D1(ptr)
#else
    #define D_LOOP
    #define D_CALL
#endif

typedef struct NAME {
    size_t priv_size;
    size_t priv_capc;
    T1*    priv_data;
} NAME;

/*
    Initialization / Destruction
*/

int FUNC_IMPL(dynarr_init)(NAME* tar, size_t inital_capacity) {
    // do not allow capacity == 0, so we dont have to check on that later
    if (inital_capacity == 0) inital_capacity = 1;
    tar->priv_size = 0;
    tar->priv_data = (T1*)A(inital_capacity * sizeof(T1));
    if (!tar->priv_data) return ERR; // return with clear state {0, 0, 0}
    tar->priv_capc = inital_capacity;
    return SCC;
}

#ifndef dynarr_init
    // Inits dynamic array with given capacity
    // May fail, O(1)
    #define dynarr_init(LSU, tar, inital_capacity) FUNC_RESP(dynarr_init, LSU)(tar, inital_capacity)
#endif

void FUNC_IMPL(dynarr_destroy)(NAME* tar) {
    D_LOOP(tar->priv_data, tar->priv_data + tar->priv_size);
    free(tar->priv_data);
    tar->priv_data = NULL;
    tar->priv_capc = 0;
    tar->priv_size = 0;
}

#ifndef dynarr_destroy
    // Properly destroys given dynamic array
    // May fail, O(n) if destructor definied, O(1) otherwise
    #define dynarr_destroy(LSU, tar) FUNC_RESP(dynarr_destroy, LSU)(tar)
#endif

/*
    Memory
*/

// dynarr_resize
// dynarr_reserve
// dynarr_shrink_to_fit

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
    #define dynarr_access(LSU, tar) FUNC_RESP(dynarr_access, LSU)(tar)
#endif

const T1* FUNC_IMPL(dynarr_const_access)(const NAME* arr) {
    return (const T1*)arr->priv_data;
}

#ifndef dynarr_const_access
    // Returns pointer to first element of contiguous block of memory, the array
    // Only the first dynarr_size() amount of elements are initialized and valid
    // O(1)
    #define dynarr_const_access(LSU, tar) FUNC_RESP(dynarr_const_access, LSU)(tar)
#endif

/*
    Operations
*/

int FUNC_IMPL(dynarr_push)(NAME* arr, T1 value) {
    if (arr->priv_size >= arr->priv_capc) {
        // Resize: double capacity
        arr->priv_capc *= 2;
        // all objects transfered
        void* new_data = realloc(arr->priv_data, arr->priv_capc * sizeof(T1));
        if (!new_data) return ERR; // allocation failed
        arr->priv_data = new_data;
    }
    ((T1*)arr->priv_data)[arr->priv_size++] = value;
    return SCC;
}

#ifndef dynarr_push
    // This operation push new element into dynamic array
    // May cause reallocation of dynamic array memory - watch out for your pointers
    // May fail, O(1) average
    #define dynarr_push(LSU, tar, value) FUNC_RESP(dynarr_push, LSU)(tar, value)
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
    #define dynarr_pop(LSU, tar, out) FUNC_RESP(dynarr_pop, LSU)(tar, out)
#endif

#undef D_CALL
#undef D_LOOP
#undef NAME
