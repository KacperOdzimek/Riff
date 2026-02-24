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

#ifndef queue
    /*
        Queue (queue)
        Standard queue structure, implemented as circular buffer, O(1) push and pop
        O(n) memory complexity
    */
    #define queue(inst) RIFF_INST(queue, inst)
#endif

typedef struct queue(INSTANCE) {
    size_t  priv_front; // inc
    size_t  priv_end;   // inc
    size_t  priv_capc;
    STORED* priv_data;
} queue(INSTANCE);

/*
    Zero / Destruction
*/

RIFF_API(void) RIFF_INST(queue_zero, INSTANCE)(queue(INSTANCE)* tar) {
    tar->priv_front = 0;
    tar->priv_end   = 0;
    tar->priv_capc  = 0;
    tar->priv_data = NULL;
}

#ifndef queue_zero
    // Makes unitialized memory proper 0-initialized empty queue
    // Does not free anything
    #define queue_zero(inst) RIFF_INST(queue_zero, inst)
#endif

RIFF_API(void) RIFF_INST(queue_destroy, INSTANCE)(queue(INSTANCE)* tar) {
    if (tar->priv_data) {
        while (tar->priv_front != tar->priv_end) {
            DESTRUCTOR(&tar->priv_data[tar->priv_front]);
            tar->priv_front = (tar->priv_front + 1) % tar->priv_capc;
        }
    }

    RIFF_FREE(tar->priv_data);
    queue_zero(INSTANCE)(tar);
}

#ifndef queue_destroy
    // Properly destroys given queue
    // O(n) if destructor definied, O(1) otherwise
    #define queue_destroy(inst) RIFF_INST(queue_destroy, inst)
#endif

/*
    Query
*/

RIFF_API(int) RIFF_INST(queue_empty, INSTANCE)(queue(INSTANCE)* tar) {
    return tar->priv_front == tar->priv_end;
}

#ifndef queue_empty
    // Returns whether the queue is empty
    // O(1)
    #define queue_empty(inst) RIFF_INST(queue_empty, inst)
#endif

RIFF_API(size_t) RIFF_INST(queue_size, INSTANCE)(queue(INSTANCE)* tar) {
    return (tar->priv_front <= tar->priv_end) ?
        (tar->priv_end - tar->priv_front) :                 // when not wrapped
        (tar->priv_capc - tar->priv_front + tar->priv_end); // when wrapped
}

#ifndef queue_size
    // Returns amount of elements inside queue
    // O(1)
    #define queue_size(inst) RIFF_INST(queue_size, inst)
#endif

/*
    Operations
*/

RIFF_API(int) RIFF_INST(queue_push, INSTANCE)(queue(INSTANCE)* tar, STORED val) {
    // queue 0-init
    if (tar->priv_capc == 0) {
        size_t  new_capc = 4;
        STORED* new_data = RIFF_ALLOC(new_capc * sizeof(STORED));

        if (!new_data) return ERR;

        tar->priv_capc = new_capc;
        tar->priv_data = new_data;
    }
    // queue full, realloc
    else if ((tar->priv_end + 1) % tar->priv_capc == tar->priv_front) {
        // can realloc without invalidation (elements order == memory order), O(1) case
        if (tar->priv_front <= tar->priv_end) {
            size_t  new_capc = tar->priv_capc * 2;
            STORED* new_data = RIFF_REALLOC(tar->priv_data, new_capc * sizeof(STORED));

            if (!new_data) return ERR;
            
            tar->priv_capc = new_capc;
            tar->priv_data = new_data;
        }
        // cannot call realloc without invalidation O(n) case
        else {
            size_t  new_capc = tar->priv_capc * 2;
            STORED* new_data = RIFF_ALLOC(new_capc * sizeof(STORED));

            if (!new_data) return ERR;
            
            // move elements
            size_t size = queue_size(INSTANCE)(tar);
            for (size_t i = 0; i < size; i++) new_data[i] = tar->priv_data[(tar->priv_front + i) % tar->priv_capc];
            
            RIFF_FREE(tar->priv_data);

            tar->priv_data  = new_data;
            tar->priv_capc  = new_capc;
            tar->priv_front = 0;
            tar->priv_end   = size;
        }
    }

    // push back
    tar->priv_data[tar->priv_end] = val;
    tar->priv_end = (tar->priv_end + 1) % tar->priv_capc;

    return SCC;
}

#ifndef queue_push
    // Pushes element at the queue's end
    // May fail (if need to alloc/realloc circular buffer), O(1) avg
    #define queue_push(inst) RIFF_INST(queue_push, inst)
#endif

RIFF_API(STORED*) RIFF_INST(queue_top, INSTANCE)(queue(INSTANCE)* tar) {
    if (queue_empty(INSTANCE)(tar)) return NULL;
    return &tar->priv_data[tar->priv_front];
}

#ifndef queue_top
    // Returns pointer to the element at the queue's front
    // Retruns NULL if no queue empty
    // O(1)
    #define queue_top(inst) RIFF_INST(queue_top, inst)
#endif

RIFF_API(int) RIFF_INST(queue_pop, INSTANCE)(queue(INSTANCE)* tar, STORED* out) {
    if (queue_empty(INSTANCE)(tar)) return ERR;

    // tranfer or destroy
    if (out) *out = tar->priv_data[tar->priv_front];
    else     DESTRUCTOR(&tar->priv_data[tar->priv_front]);

    // move on
    tar->priv_front++;
    tar->priv_front %= tar->priv_capc;

    return SCC;
}

#ifndef queue_pop
    // Pops out the front element from the queue 
    // If   out == NULL the element will be destructed
    // Else *out = element and the caller does own the element on from now
    // May fail (empty queue), O(1)
    #define queue_pop(inst) RIFF_INST(queue_pop, inst)
#endif
