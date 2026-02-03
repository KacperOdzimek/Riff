/*
Mandatory macro params:
    SU - name suffix
    T1 - stored type
    F1 - T1(func)(T1 l, T1 r) - tree associative function: sum, min, max, ...

Optional macro params:
    A  - custom allocator, same signature as malloc (defaults to malloc)
    F  - custom matching free                       (defaults to free)

NO DESTRUCTOR!
    This container is designed to operate on primitive types, 
    that do NOT have bound resources (like numbers)
    Therefore there are no destructor calls
*/

/*
    Segment Tree
    The most standard point-segment tree
    Allows for O(log n) queries for sums, maxes, mins on ranges and in points
    The initial range is rouned to next power of two
*/

#include "generic.h"

#define NAME NAME_MAKE(segtree, SU)

typedef struct NAME {
    size_t priv_base_size;
    T1*    priv_memory;
    T1     priv_default_val;
} NAME;

/*
    Initialization / Destruction
*/

int FUNC_IMPL(segtree_init)(NAME* tar, size_t range_size, T1 default_val) {
    size_t base = 1;
    while (base < range_size) base *= 2;

    tar->priv_memory = A(2 * base * sizeof(T1));
    if (!tar->priv_memory) return ERR; // allocation failure

    for (size_t i = 0; i < 2 * base; i++) {
        tar->priv_memory[i] = default_val;
    }

    tar->priv_base_size = base;
    tar->priv_default_val = default_val;
    return SCC;
}

#ifndef segtree_init
    // Inits segtree of given size
    // O(n)
    #define segtree_init(LSU) FUNC_RESP(segtree_init, LSU)
#endif

void FUNC_IMPL(segtree_destroy)(NAME* tar) {
    F(tar->priv_memory);
    tar->priv_base_size = 0;
}

#ifndef segtree_destroy
    // Properly destroys given segtree
    // O(1)
    #define segtree_destroy(LSU) FUNC_RESP(segtree_destroy, LSU)
#endif

/*
    Getters
*/

size_t FUNC_IMPL(segtree_base_size)(NAME* tar) {
    return tar->priv_base_size;
}

#ifndef segtree_base_size
    // Returns base size of the segment tree
    // All operations and quary calls must have indicies < this
    // O(1)
    #define segtree_base_size(LSU) FUNC_RESP(segtree_base_size, LSU);
#endif

/*
    Operations
*/

void FUNC_IMPL(segtree_change)(NAME* tar, size_t pos, T1 val) {
    pos += tar->priv_base_size;
    tar->priv_memory[pos] = val;
    pos /= 2;

    while (pos) {
        tar->priv_memory[pos] = F1(
            tar->priv_memory[pos * 2], 
            tar->priv_memory[pos * 2 + 1]
        );
        pos /= 2;
    }
}

#ifndef segtree_change
    // Changes value of segtree in given position
    // Position must be < segtree_base_size
    // O(log n)
    #define segtree_change(LSU) FUNC_RESP(segtree_change, LSU)
#endif

/*
    Queries
*/

T1 FUNC_IMPL(segtree_query_top)(NAME* tar) {
    return tar->priv_memory[1];
}

#ifndef segtree_query_top
    // Returns top value of the tree
    // O(1)
    #define segtree_query_top(LSU) FUNC_RESP(segtree_query_top, LSU)
#endif

T1 FUNC_IMPL(segtree_query_range)(NAME* tar, size_t beg, size_t end) {
    T1 res_l = tar->priv_default_val;
    T1 res_r = tar->priv_default_val;

    beg += tar->priv_base_size;
    end += tar->priv_base_size;

    while (beg < end) {
        if (beg & 1) {
            res_l = F1(res_l, tar->priv_memory[beg]);
            beg++;
        }
        if (end & 1) {
            end--;
            res_r = F1(tar->priv_memory[end], res_r);
        }
        beg /= 2; end /= 2;
    }

    return F1(res_l, res_r);
}

#ifndef segtree_query_range
    // By comparing values with F1, find answer for range [beg, end)
    // O(log n)
    #define segtree_query_range(LSU) FUNC_RESP(segtree_query_range, LSU)
#endif
