/*
Mandatory macro params:
    SU - name  suffix
    T1 - key   type
    T2 - value type
    F1 - key type hash  function - size_t(func)(const T1*)
    F2 - key type equal function - int(func)(const T1* a, const T1* b)

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

typedef struct NAME {


    size_t  priv_size;  // actual count of items within
    size_t  priv_capc;  // size of arrays
} NAME;

/*
    Initialization / Destruction
*/

int FUNC_IMPL(hashmap_init)(NAME* tar) {

}

#ifndef hashmap_init
    // todo
    #define hashmap_init(LSU) FUNC_RESP(hashmap_init, LSU)
#endif

void FUNC_IMPL(hashmap_destroy)(NAME* tar) {

}

#ifndef hashmap_destroy
    // todo
    #define hashmap_destroy(LSU) FUNC_RESP(hashmap_destroy, LSU)
#endif

/*
    Memory
*/

int FUNC_IMPL(hashmap_rehash)(NAME* tar) {

}

#ifndef hashmap_rehash
    // todo
    #define hashmap_rehash(LSU) FUNC_RESP(hashmap_rehash, LSU)
#endif

/*
    Operations
*/

int FUNC_IMPL(hashmap_find)(NAME* tar, const T1* user_key, const T1** key, T2** value) {

}

#ifndef hashmap_find
    // todo
    #define hashmap_find(LSU) FUNC_RESP(hashmap_find, LSU)
#endif


int FUNC_IMPL(hashmap_pop)(NAME* tar, const T1* key, T2* out) {

}

#ifndef hashmap_pop
    // todo
    #define hashmap_pop(LSU) FUNC_RESP(hashmap_pop, LSU)
#endif


int FUNC_IMPL(hashmap_push)(NAME* tar, T1 key, T2 value) {

}

#ifndef hashmap_push
    // todo
    #define hashmap_push(LSU) FUNC_RESP(hashmap_push, LSU)
#endif


#undef KEY_D_CALL
#undef VAL_D_CALL
#undef NAME
