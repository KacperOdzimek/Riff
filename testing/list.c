#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Destructor for elements */
void dest(int* ptr) {
    printf("Destructed: %d\n", *ptr);
}

#define SU int
#define T1 int
#define D1 dest
#include "cup/list.h"
#include "cup/undef.h"

/* Helper: print list forward */
void print_list(list_int* list) {
    printf("List (size=%zu): ", list_size(int)(list));

    for (list_node_int* n = list_first(int)(list); n != NULL; n = list_next(int)(n)) {
        printf("%d ", *list_const_access(int)(n));
    }

    printf("\n");
}

/* Helper: validate expected order */
void expect_order(list_int* list, int* arr, size_t count) {
    assert(list_size(int)(list) == count);

    size_t i = 0;
    for (list_node_int* n = list_first(int)(list); n != NULL; n = list_next(int)(n)) {
        assert(*list_const_access(int)(n) == arr[i]);
        i++;
    }

    assert(i == count);
}

/* Main test */
int main() {
    list_int list;
    list_init(int)(&list);

    printf("=== Test 0: empty list ===\n");
    print_list(&list);

    /* Insert into empty list using push_before(NULL) -> push_back */
    printf("\n=== Test 1: push_before(NULL) into empty ===\n");
    list_node_int* n1 = list_push_before(int)(&list, NULL, 10);
    assert(n1 != NULL);

    int exp1[] = {10};
    expect_order(&list, exp1, 1);
    print_list(&list);

    /* Insert at front using push_after(NULL) -> push_front */
    printf("\n=== Test 2: push_after(NULL) -> push_front ===\n");
    list_node_int* n2 = list_push_after(int)(&list, NULL, 5);
    assert(n2 != NULL);

    int exp2[] = {5, 10};
    expect_order(&list, exp2, 2);
    print_list(&list);

    /* Insert at end using push_before(NULL) -> push_back */
    printf("\n=== Test 3: push_before(NULL) -> push_back ===\n");
    list_node_int* n3 = list_push_before(int)(&list, NULL, 20);
    assert(n3 != NULL);

    int exp3[] = {5, 10, 20};
    expect_order(&list, exp3, 3);
    print_list(&list);

    /* Insert before middle node */
    printf("\n=== Test 4: insert before middle ===\n");
    list_node_int* mid = list_next(int)(list_first(int)(&list)); // node with 10
    list_node_int* n4 = list_push_before(int)(&list, mid, 7);
    assert(n4 != NULL);

    int exp4[] = {5, 7, 10, 20};
    expect_order(&list, exp4, 4);
    print_list(&list);

    /* Insert after last node */
    printf("\n=== Test 5: insert after last ===\n");
    list_node_int* last = list_last(int)(&list);
    list_node_int* n5 = list_push_after(int)(&list, last, 30);
    assert(n5 != NULL);

    int exp5[] = {5, 7, 10, 20, 30};
    expect_order(&list, exp5, 5);
    print_list(&list);

    /* Insert before first */
    printf("\n=== Test 6: insert before first ===\n");
    list_node_int* first = list_first(int)(&list);
    list_node_int* n6 = list_push_before(int)(&list, first, 1);
    assert(n6 != NULL);

    int exp6[] = {1, 5, 7, 10, 20, 30};
    expect_order(&list, exp6, 6);
    print_list(&list);

    printf("\n=== All insertion tests passed ===\n");

    /* Destroy list and confirm destructor calls */
    printf("\n=== Destroying list ===\n");
    list_destroy(int)(&list);

    return 0;
}
