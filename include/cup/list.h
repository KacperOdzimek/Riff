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
    Double-sided linked list
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
    T1    priv_obj;
    NODE* priv_prev;
    NODE* priv_next;
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

int FUNC_IMPL(list_rim)(NODE* n) {
    return (n == NULL);
}

#ifndef list_rim
    #define list_rim(LSU) FUNC_RESP(list_prev, LSU);
#endif

NODE* FUNC_IMPL(list_next)(NODE* n) {
    return n->priv_next;
}

#ifndef list_next
    #define list_next(LSU) FUNC_RESP(list_next, LSU)
#endif

NODE* FUNC_IMPL(list_prev)(NODE* n) {
    return n->priv_prev;
}

#ifndef list_prev
    #define list_prev(LSU) FUNC_RESP(list_prev, LSU)
#endif

#undef D_CALL
#undef NODE
#undef NAME
