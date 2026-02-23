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

/*
    Typedef
*/

#ifndef dlst_node
    /*
        Double linked list node
    */
    #define dlst_node(inst) RIFF_INST(dlst_node, inst)
#endif

#ifndef dlst
    /*
        Double-linked list (dlst)
        O(n) memory complexity
    */
    #define dlst(inst) RIFF_INST(dlst, inst)
#endif

typedef struct dlst_node(INSTANCE) {
    STORED                priv_obj;
    struct dlst_node(INSTANCE)* priv_prev;
    struct dlst_node(INSTANCE)* priv_next;
} dlst_node(INSTANCE);

typedef struct dlst(INSTANCE) {
    size_t          priv_size;
    dlst_node(INSTANCE)*  priv_first;
    dlst_node(INSTANCE)*  priv_last;
} dlst(INSTANCE);

/*
    Zero / Destruction
*/

void RIFF_INST(dlst_zero, INSTANCE)(dlst(INSTANCE)* tar) {
    tar->priv_size  = 0;
    tar->priv_first = NULL;
    tar->priv_last  = NULL;
}

#ifndef dlst_zero
    // Makes uninitialized memory proper 0-initialized empty list
    // Does not free anything
    #define dlst_zero(inst) RIFF_INST(dlst_zero, inst)
#endif

void RIFF_INST(dlst_destroy, INSTANCE)(dlst(INSTANCE)* tar) {
    dlst_node(INSTANCE)* cur = tar->priv_first;
    while (cur) {
        dlst_node(INSTANCE)* next = cur->priv_next;
        DESTRUCTOR(&cur->priv_obj); // destroy object
        RIFF_FREE(cur); // free node
        cur = next;
    }

    dlst_zero(INSTANCE)(tar);
}

#ifndef dlst_destroy
    // Free allocated memory, destroys owned objects
    #define dlst_destroy(inst) RIFF_INST(dlst_destroy, inst)
#endif

/*
    Getters
*/

size_t RIFF_INST(dlst_size, INSTANCE)(const dlst(INSTANCE)* tar) {
    return tar->priv_size;
}

#ifndef dlst_size
    // Returns list size
    // O(1)
    #define dlst_size(inst) RIFF_INST(dlst_size, inst)
#endif

/*
    Node Operations
*/

dlst_node(INSTANCE)* RIFF_INST(dlst_first, INSTANCE)(const dlst(INSTANCE)* tar) {
    return tar->priv_first;
}

#ifndef dlst_first 
    // Returns pointer to the first node in the list
    // NULL if list is empty
    // O(1)
    #define dlst_first(inst) RIFF_INST(dlst_first, inst)
#endif

dlst_node(INSTANCE)* RIFF_INST(dlst_last, INSTANCE)(const dlst(INSTANCE)* tar) {
    return tar->priv_last;
}

#ifndef dlst_last
    // Returns pointer to the last node in the list
    // NULL if list is empty
    // O(1)
    #define dlst_last(inst) RIFF_INST(dlst_last, inst)
#endif

dlst_node(INSTANCE)* RIFF_INST(dlst_next, INSTANCE)(dlst_node(INSTANCE)* n) {
    return n ? n->priv_next : NULL;
}

#ifndef dlst_next
    // Given pointer to a list's node, returns pointer to the next list's node
    // Returns NULL if given pointer was last node in the list
    // Returns NULL if passed NULL
    // O(1)
    #define dlst_next(inst) RIFF_INST(dlst_next, inst)
#endif

dlst_node(INSTANCE)* RIFF_INST(dlst_prev, INSTANCE)(dlst_node(INSTANCE)* n) {
    return n ? n->priv_prev : NULL;
}

#ifndef dlst_prev
    // Given pointer to a list's node, returns pointer to the previous list's node
    // Returns NULL if given pointer was first node in the list
    // Returns NULL if passed NULL
    // O(1)
    #define dlst_prev(inst) RIFF_INST(dlst_prev, inst)
#endif

STORED* RIFF_INST(dlst_access, INSTANCE)(dlst_node(INSTANCE)* n) {
    return &n->priv_obj;
}

#ifndef dlst_access
    // Returns pointer to the object stored inside the given node
    // Do not invalidate the object, as the destructor (if provided) will be called on it sooner or later
    // O(1)
    #define dlst_access(inst) RIFF_INST(dlst_access, inst)
#endif

const STORED* RIFF_INST(dlst_const_access, INSTANCE)(dlst_node(INSTANCE)* n) {
    return &n->priv_obj;
}

#ifndef dlst_const_access
    // Returns read only pointer to the object stored inside the given node
    // O(1)
    #define dlst_const_access(inst) RIFF_INST(dlst_const_access, inst)
#endif

/*
    Operations
*/

dlst_node(INSTANCE)* RIFF_INST(dlst_push_before, INSTANCE)(dlst(INSTANCE)* tar, dlst_node(INSTANCE)* before, STORED value) {
    dlst_node(INSTANCE)* new_node = (dlst_node(INSTANCE)*)RIFF_ALLOC(sizeof(dlst_node(INSTANCE)));
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

#ifndef dlst_push_before
    // Inserts new node with given value before the "before" node
    // If "before" node is NULL new node will be now the last one 
    // (it gets pushed before exclusive list end)
    // Takes ownership of object at success
    // May fail allocation, returns NULL on fail, valid node pointer otherwise, O(1)
    #define dlst_push_before(inst) RIFF_INST(dlst_push_before, inst)
#endif

dlst_node(INSTANCE)* RIFF_INST(dlst_push_after, INSTANCE)(dlst(INSTANCE)* tar, dlst_node(INSTANCE)* after, STORED value) {
    dlst_node(INSTANCE)* new_node = (dlst_node(INSTANCE)*)RIFF_ALLOC(sizeof(dlst_node(INSTANCE)));
    if (!new_node) return NULL;

    new_node->priv_obj = value;

    // Insert at begin if after == NULL
    if (after == NULL) {
        new_node->priv_prev = NULL;
        new_node->priv_next = tar->priv_first;

        if (tar->priv_first) tar->priv_first->priv_prev = new_node;
        else                 tar->priv_last = new_node; // dlst was empty

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

#ifndef dlst_push_after
    // Inserts new node with given value after the "after" node
    // If "after" node is NULL new node will be now the first one
    // (it gets pushed after exclusive list begin)
    // Takes ownership of object at success
    // May fail allocation, returns NULL on fail, node valid pointer otherwise, O(1)
    #define dlst_push_after(inst) RIFF_INST(dlst_push_after, inst)
#endif

void RIFF_INST(dlst_pop, INSTANCE)(dlst(INSTANCE)* tar, dlst_node(INSTANCE)* n, STORED* out) {
    // get rid of object
    if (out) *out = n->priv_obj;
    else DESTRUCTOR(&n->priv_obj);

    // Relink previous node
    if (n->priv_prev) n->priv_prev->priv_next = n->priv_next;
    else              tar->priv_first = n->priv_next; // n was first node
    
    // Relink next node
    if (n->priv_next) n->priv_next->priv_prev = n->priv_prev;
    else              tar->priv_last = n->priv_prev; // n was last node

    // Decrease size and free
    tar->priv_size--;
    RIFF_FREE(n);
}

#ifndef dlst_pop
    // Erases given element from the list
    // If out == NULL destructor will be called on contained object
    // Otherwise *out = element, caller gets ownership over object and destructor will not be called
    // O(1)
    #define dlst_pop(inst) RIFF_INST(dlst_pop, inst)
#endif

void RIFF_INST(dlst_clear, INSTANCE)(dlst(INSTANCE)* tar) {
    dlst_destroy(INSTANCE)(tar); // apparently the same
}

#ifndef dlst_clear
    #define dlst_clear(inst) RIFF_INST(dlst_clear, inst)
#endif

#undef INSTANCE
#undef STORED
#undef DESTRUCTOR

// consume parameters
#undef T
#undef A
