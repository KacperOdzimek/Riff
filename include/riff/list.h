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

#ifndef list_node
    #define list_node(LSU) NAME(list_node, LSU)
#endif

#ifndef list
    #define list(LSU) NAME(list, LSU)
#endif

typedef struct list_node(SU) {
    T1                    priv_obj;
    struct list_node(SU)* priv_prev;
    struct list_node(SU)* priv_next;
} list_node(SU);

typedef struct list(SU) {
    size_t          priv_size;
    list_node(SU)*  priv_first;
    list_node(SU)*  priv_last;
} list(SU);

/*
    Helpers
*/

#ifdef D1
    #define D_CALL(ptr) D1(ptr)
#else
    #define D_CALL(ptr) ((void)0)
#endif

/*
    Zero / Destruction
*/

void IMPL(list_zero, SU)(list(SU)* tar) {
    tar->priv_size  = 0;
    tar->priv_first = NULL;
    tar->priv_last  = NULL;
}

#ifndef list_zero
    // Makes unitialized memory proper 0-initialized empty list
    // Does not free anything
    #define list_zero(LSU) IMPL(list_zero, LSU)
#endif

void IMPL(list_destroy, SU)(list(SU)* tar) {
    list_node(SU)* cur = tar->priv_first;
    while (cur) {
        list_node(SU)* next = cur->priv_next;
        D_CALL(&cur->priv_obj); // destroy object
        F(cur); // free node
        cur = next;
    }

    list_zero(SU)(tar);
}

#ifndef list_destroy
    // Free allocated memory, destroys owned objects
    #define list_destroy(LSU) IMPL(list_destroy, LSU)
#endif

/*
    Getters
*/

size_t IMPL(list_size, SU)(const list(SU)* tar) {
    return tar->priv_size;
}

#ifndef list_size
    // Returns list size
    // O(1)
    #define list_size(LSU) IMPL(list_size, LSU)
#endif

/*
    Node Operations
*/

list_node(SU)* IMPL(list_first, SU)(const list(SU)* tar) {
    return tar->priv_first;
}

#ifndef list_first 
    // Returns pointer to the first node in the list
    // NULL if list is empty
    // O(1)
    #define list_first(LSU) IMPL(list_first, LSU)
#endif

list_node(SU)* IMPL(list_last, SU)(const list(SU)* tar) {
    return tar->priv_last;
}

#ifndef list_last
    // Returns pointer to the last node in the list
    // NULL if list is empty
    // O(1)
    #define list_last(LSU) IMPL(list_last, LSU)
#endif

list_node(SU)* IMPL(list_next, SU)(list_node(SU)* n) {
    return n ? n->priv_next : NULL;
}

#ifndef list_next
    // Given pointer to a list's node, returns pointer to the next list's node
    // Returns NULL if given pointer was last node in the list
    // Returns NULL if passed NULL
    // O(1)
    #define list_next(LSU) IMPL(list_next, LSU)
#endif

list_node(SU)* IMPL(list_prev, SU)(list_node(SU)* n) {
    return n ? n->priv_prev : NULL;
}

#ifndef list_prev
    // Given pointer to a list's node, returns pointer to the previous list's node
    // Returns NULL if given pointer was first node in the list
    // Returns NULL if passed NULL
    // O(1)
    #define list_prev(LSU) IMPL(list_prev, LSU)
#endif

T1* IMPL(list_access, SU)(list_node(SU)* n) {
    return &n->priv_obj;
}

#ifndef list_access
    // Returns pointer to the object stored inside the given node
    // Do not invalidate the object, as the destructor (if provided) will be called on it sooner or later
    // O(1)
    #define list_access(LSU) IMPL(list_access, LSU)
#endif

const T1* IMPL(list_const_access, SU)(list_node(SU)* n) {
    return &n->priv_obj;
}

#ifndef list_const_access
    // Returns read only pointer to the object stored inside the given node
    // O(1)
    #define list_const_access(LSU) IMPL(list_const_access, LSU)
#endif

/*
    Operations
*/

list_node(SU)* IMPL(list_push_before, SU)(list(SU)* tar, list_node(SU)* before, T1 value) {
    list_node(SU)* new_node = (list_node(SU)*)A(sizeof(list_node(SU)));
    if (!new_node) return NULL;

    new_node->priv_obj = value;

    // Insert at end if before == NULL
    if (before == NULL) {
        new_node->priv_prev = tar->priv_last;
        new_node->priv_next = NULL;

        if (tar->priv_last) tar->priv_last->priv_next = new_node;
        else                tar->priv_first = new_node;

        tar->priv_last = new_node;
    }
    // Insert before given list node
    else {    
        new_node->priv_prev = before->priv_prev;
        new_node->priv_next = before;

        if (before->priv_prev) before->priv_prev->priv_next = new_node;
        else                   tar->priv_first = new_node;

        before->priv_prev = new_node;
    }

    tar->priv_size++;
    return new_node;
}

#ifndef list_push_before
    // Inserts new node with given value before the "before" node
    // If "before" node is NULL new node will be now the last one 
    // (it gets pushed before exclusive list end)
    // Takes ownership of object at success
    // May fail allocation, returns NULL on fail, valid node pointer otherwise, O(1)
    #define list_push_before(LSU) IMPL(list_push_before, LSU)
#endif

list_node(SU)* IMPL(list_push_after, SU)(list(SU)* tar, list_node(SU)* after, T1 value) {
    list_node(SU)* new_node = (list_node(SU)*)A(sizeof(list_node(SU)));
    if (!new_node) return NULL;

    new_node->priv_obj = value;

    // Insert at begin if after == NULL
    if (after == NULL) {
        new_node->priv_prev = NULL;
        new_node->priv_next = tar->priv_first;

        if (tar->priv_first) tar->priv_first->priv_prev = new_node;
        else                 tar->priv_last = new_node; // list was empty

        tar->priv_first = new_node;
    }
    else {
        // Insert after given node
        new_node->priv_prev = after;
        new_node->priv_next = after->priv_next;

        if (after->priv_next) after->priv_next->priv_prev = new_node;
        else                  tar->priv_last = new_node;  // inserting after last node

        after->priv_next = new_node;
    }

    tar->priv_size++;
    return new_node;
}

#ifndef list_push_after
    // Inserts new node with given value after the "after" node
    // If "after" node is NULL new node will be now the first one
    // (it gets pushed after exclusive list begin)
    // Takes ownership of object at success
    // May fail allocation, returns NULL on fail, node valid pointer otherwise, O(1)
    #define list_push_after(LSU) IMPL(list_push_after, LSU)
#endif

void IMPL(list_pop, SU)(list(SU)* tar, list_node(SU)* n, T1* out) {
    // get rid of object
    if (out) *out = n->priv_obj;
    else D_CALL(&n->priv_obj);

    // Relink previous node
    if (n->priv_prev) n->priv_prev->priv_next = n->priv_next;
    else              tar->priv_first = n->priv_next; // n was first node
    
    // Relink next node
    if (n->priv_next) n->priv_next->priv_prev = n->priv_prev;
    else              tar->priv_last = n->priv_prev; // n was last node

    // Decrease size and free
    tar->priv_size--;
    F(n);
}

#ifndef list_pop
    // Erases given element from the list
    // If out == NULL destructor will be called on contained object
    // Otherwise *out = element, caller gets ownership over object and destructor will not be called
    // O(1)
    #define list_pop(LSU) IMPL(list_pop, LSU)
#endif

void IMPL(list_clear, SU)(list(SU)* tar) {
    list_destroy(SU)(tar); // apparently the same
}

#ifndef list_clear
    #define list_clear(LSU) IMPL(list_clear, LSU)
#endif

#undef D_CALL
