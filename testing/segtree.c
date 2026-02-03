#include <stdio.h>
#include <stdlib.h>

int add(int a, int b) {
    return a + b;
}

#define SU int
#define T1 int
#define F1 add
#include "cup/segtree.h"

int main() {
    segtree_int tree;

    // 1) Init with size 8 and default value 0
    if (segtree_init(int)(&tree, 8, 0) == ERR) {
        printf("Init failed\n");
        return 1;
    }

    // 2) Update some values
    segtree_change(int)(&tree, 0, 5);
    segtree_change(int)(&tree, 1, 3);
    segtree_change(int)(&tree, 2, 7);
    segtree_change(int)(&tree, 3, 1);
    segtree_change(int)(&tree, 4, 9);

    // 3) Range queries (half-open [l, r))
    printf("Sum [0, 5): %d (expected 25)\n", segtree_query_range(int)(&tree, 0, 5));
    printf("Sum [1, 4): %d (expected 11)\n", segtree_query_range(int)(&tree, 1, 4));
    printf("Sum [2, 3): %d (expected 7)\n",  segtree_query_range(int)(&tree, 2, 3));
    printf("Sum [0, 8): %d (expected 25)\n", segtree_query_range(int)(&tree, 0, 8));

    // 4) Update and re-query
    segtree_change(int)(&tree, 2, 10);
    printf("After update, sum [0, 5): %d (expected 28)\n",
           segtree_query_range(int)(&tree, 0, 5));

    // 5) Clean up
    segtree_destroy(int)(&tree);
    return 0;
}
