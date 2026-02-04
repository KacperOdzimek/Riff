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
    O(1) memory complexity
*/

#include "generic.h"

#define NAME NAME_MAKE(list, SU)
#define ELEM NAME_MAKE(list_elem, SU);

#ifdef D1
    #define D_CALL(ptr) D1(ptr)
#else
    #define D_CALL
#endif

typedef struct ELEM {
    ELEM* priv_prev;
    ELEM* priv_next;
    T1*   priv_elem;
};

typedef struct NAME {
    
} NAME;

#undef D_CALL
#undef ELEM
#undef NAME
