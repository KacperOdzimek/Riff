/*
    List
    Double-linked list
    O(n) memory complexity
*/

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

/*
    Typedef
*/

#ifndef list_node
    #define list_node(inst) RIFF_NAME(list_node, inst)
#endif

#ifndef list
    #define list(inst) RIFF_NAME(list, inst)
#endif

typedef struct list_node(INSTANCE) {
    STORED                priv_obj;
    struct list_node(INSTANCE)* priv_prev;
    struct list_node(INSTANCE)* priv_next;
} list_node(INSTANCE);

typedef struct list(INSTANCE) {
    size_t          priv_size;
    list_node(INSTANCE)*  priv_first;
    list_node(INSTANCE)*  priv_last;
} list(INSTANCE);

/*
    Zero / Destruction
*/

void RIFF_INST(list_zero, INSTANCE)(list(INSTANCE)* tar) {
    tar->priv_size  = 0;
    tar->priv_first = NULL;
    tar->priv_last  = NULL;
}

#ifndef list_zero
    // Makes unitialized memory proper 0-initialized empty list
    // Does not free anything
    #define list_zero(inst) RIFF_INST(list_zero, inst)
#endif

void RIFF_INST(list_destroy, INSTANCE)(list(INSTANCE)* tar) {
    list_node(INSTANCE)* cur = tar->priv_first;
    while (cur) {
        list_node(INSTANCE)* next = cur->priv_next;
        DESTRUCTOR(&cur->priv_obj); // destroy object
        RIFF_FREE(cur); // free node
        cur = next;
    }

    list_zero(INSTANCE)(tar);
}

#ifndef list_destroy
    // Free allocated memory, destroys owned objects
    #define list_destroy(inst) RIFF_INST(list_destroy, inst)
#endif

/*
    Getters
*/

size_t RIFF_INST(list_size, INSTANCE)(const list(INSTANCE)* tar) {
    return tar->priv_size;
}

#ifndef list_size
    // Returns list size
    // O(1)
    #define list_size(inst) RIFF_INST(list_size, inst)
#endif

/*
    Node Operations
*/

list_node(INSTANCE)* RIFF_INST(list_first, INSTANCE)(const list(INSTANCE)* tar) {
    return tar->priv_first;
}

#ifndef list_first 
    // Returns pointer to the first node in the list
    // NULL if list is empty
    // O(1)
    #define list_first(inst) RIFF_INST(list_first, inst)
#endif

list_node(INSTANCE)* RIFF_INST(list_last, INSTANCE)(const list(INSTANCE)* tar) {
    return tar->priv_last;
}

#ifndef list_last
    // Returns pointer to the last node in the list
    // NULL if list is empty
    // O(1)
    #define list_last(inst) RIFF_INST(list_last, inst)
#endif

list_node(INSTANCE)* RIFF_INST(list_next, INSTANCE)(list_node(INSTANCE)* n) {
    return n ? n->priv_next : NULL;
}

#ifndef list_next
    // Given pointer to a list's node, returns pointer to the next list's node
    // Returns NULL if given pointer was last node in the list
    // Returns NULL if passed NULL
    // O(1)
    #define list_next(inst) RIFF_INST(list_next, inst)
#endif

list_node(INSTANCE)* RIFF_INST(list_prev, INSTANCE)(list_node(INSTANCE)* n) {
    return n ? n->priv_prev : NULL;
}

#ifndef list_prev
    // Given pointer to a list's node, returns pointer to the previous list's node
    // Returns NULL if given pointer was first node in the list
    // Returns NULL if passed NULL
    // O(1)
    #define list_prev(inst) RIFF_INST(list_prev, inst)
#endif

STORED* RIFF_INST(list_access, INSTANCE)(list_node(INSTANCE)* n) {
    return &n->priv_obj;
}

#ifndef list_access
    // Returns pointer to the object stored inside the given node
    // Do not invalidate the object, as the destructor (if provided) will be called on it sooner or later
    // O(1)
    #define list_access(inst) RIFF_INST(list_access, inst)
#endif

const STORED* RIFF_INST(list_const_access, INSTANCE)(list_node(INSTANCE)* n) {
    return &n->priv_obj;
}

#ifndef list_const_access
    // Returns read only pointer to the object stored inside the given node
    // O(1)
    #define list_const_access(inst) RIFF_INST(list_const_access, inst)
#endif

/*
    Operations
*/

list_node(INSTANCE)* RIFF_INST(list_push_before, INSTANCE)(list(INSTANCE)* tar, list_node(INSTANCE)* before, STORED value) {
    list_node(INSTANCE)* new_node = (list_node(INSTANCE)*)RIFF_ALLOC(sizeof(list_node(INSTANCE)));
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
    #define list_push_before(inst) RIFF_INST(list_push_before, inst)
#endif

list_node(INSTANCE)* RIFF_INST(list_push_after, INSTANCE)(list(INSTANCE)* tar, list_node(INSTANCE)* after, STORED value) {
    list_node(INSTANCE)* new_node = (list_node(INSTANCE)*)RIFF_ALLOC(sizeof(list_node(INSTANCE)));
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
    #define list_push_after(inst) RIFF_INST(list_push_after, inst)
#endif

void RIFF_INST(list_pop, INSTANCE)(list(INSTANCE)* tar, list_node(INSTANCE)* n, STORED* out) {
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

#ifndef list_pop
    // Erases given element from the list
    // If out == NULL destructor will be called on contained object
    // Otherwise *out = element, caller gets ownership over object and destructor will not be called
    // O(1)
    #define list_pop(inst) RIFF_INST(list_pop, inst)
#endif

void RIFF_INST(list_clear, INSTANCE)(list(INSTANCE)* tar) {
    list_destroy(INSTANCE)(tar); // apparently the same
}

#ifndef list_clear
    #define list_clear(inst) RIFF_INST(list_clear, inst)
#endif

#undef INSTANCE
#undef STORED
#undef DESTRUCTOR
