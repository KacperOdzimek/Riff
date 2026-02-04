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
    List
    Double-linked list
    O(n) memory complexity
*/

#include "generic.h"

#define NAME NAME_MAKE(list, SU)
#define NODE NAME_MAKE(list_node, SU)

#ifdef D1
    #define D_CALL(ptr) D1(ptr)
#else
    #define D_CALL
#endif

typedef struct NODE {
    T1           priv_obj;
    struct NODE* priv_prev;
    struct NODE* priv_next;
} NODE;

typedef struct NAME {
    size_t priv_size;
    NODE*  priv_first;
    NODE*  priv_last;
} NAME;

/*
    Initialization / Destruction
*/

void FUNC_IMPL(list_init)(NAME* tar) {
    tar->priv_size  = 0;
    tar->priv_first = NULL;
    tar->priv_last  = NULL;
}

#ifndef list_init
    #define list_init(LSU) FUNC_RESP(list_init, LSU)
#endif

void FUNC_IMPL(list_destroy)(NAME* tar) {
    NODE* cur = tar->priv_first;
    while (cur) {
        NODE* next = cur->priv_next;
        D_CALL(&cur->priv_obj); // destroy object
        F(cur); // free node
        cur = next; 
    }

    tar->priv_size  = 0;
    tar->priv_first = NULL;
    tar->priv_last  = NULL;
}

#ifndef list_destroy
    #define list_destroy(LSU) FUNC_RESP(list_destroy, LSU)
#endif

/*
    Node Operations
*/

NODE* FUNC_IMPL(list_first)(NAME* tar) {
    return tar->priv_first;
}

#ifndef list_first 
    // Returns pointer to the first node in the list
    // NULL if list is empty
    // O(1)
    #define list_first(LSU) FUNC_RESP(list_first, LSU)
#endif

NODE* FUNC_IMPL(list_last)(NAME* tar) {
    return tar->priv_last;
}

#ifndef list_last
    // Returns pointer to the last node in the list
    // NULL if list is empty
    // O(1)
    #define list_last(LSU) FUNC_RESP(list_last, LSU)
#endif

NODE* FUNC_IMPL(list_next)(NODE* n) {
    return n->priv_next;
}

#ifndef list_next
    // Given pointer to a list's node, returns pointer to the next list's node
    // Returns NULL if given pointer was last node in the list
    // O(1)
    #define list_next(LSU) FUNC_RESP(list_next, LSU)
#endif

NODE* FUNC_IMPL(list_prev)(NODE* n) {
    return n->priv_prev;
}

#ifndef list_prev
    // Given pointer to a list's node, returns pointer to the previous list's node
    // Returns NULL if given pointer was first node in the list
    // O(1)
    #define list_prev(LSU) FUNC_RESP(list_prev, LSU)
#endif

T1* FUNC_IMPL(list_access)(NODE* n) {
    return &n->priv_obj;
}

#ifndef list_access
    // Returns pointer to the object stored inside the given node
    // Do not invalidate the object, as the destructor (if provided) will be called on it sooner or later
    // O(1)
    #define list_access(LSU) FUNC_RESP(list_access, LSU)
#endif

const T1* FUNC_IMPL(list_const_access)(NODE* n) {
    return &n->priv_obj;
}

#ifndef list_const_access
    // Returns read only pointer to the object stored inside the given node
    // O(1)
    #define list_const_access(LSU) FUNC_RESP(list_const_access, LSU)
#endif

/*
    Push Operations
*/


#undef D_CALL
#undef NODE
#undef NAME
