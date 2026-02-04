#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ----------------------------- */
/* Dynarr API setup */

#define SU int
#define T1 int
#include "cup/dynarr.h"
#include "cup/undef.h"

/* ----------------------------- */
/* List of dynarrs setup, use dynarr_destroy as destructor */

#define SU int
#define T1 dynarr_int
#define D1 dynarr_destroy(int)   // <-- list will automatically destroy dynarrs
#include "cup/list.h"
#include "cup/undef.h"

/* ----------------------------- */
/* Test program */

int main() {
    srand((unsigned int)time(NULL));

    // Create a list of dynamic arrays
    list_int list_of_arrays;
    list_init(int)(&list_of_arrays);

    // Create 3 initial arrays and push to list
    for (int i = 0; i < 3; i++) {
        dynarr_int arr;
        if (!dynarr_init(int)(&arr, 4)) {
            printf("Failed to init dynarr\n");
            return 1;
        }
        list_push_after(int)(&list_of_arrays, NULL, arr);
    }

    // Perform 100 random operations
    for (int op = 0; op < 100; op++) {
        int action = rand() % 3;

        size_t list_size_val = list_size(int)(&list_of_arrays);
        if (list_size_val == 0) continue;

        // Pick random array from list
        size_t idx = rand() % list_size_val;
        list_node_int* n = list_first(int)(&list_of_arrays);
        for (size_t i = 0; i < idx; i++) n = list_next(int)(n);

        dynarr_int* arr_ptr = list_access(int)(n);

        if (action == 0) {
            // Push random value
            int val = rand() % 100;
            dynarr_push(int)(arr_ptr, val);
        } else if (action == 1) {
            // Pop value if array not empty
            if (dynarr_size(int)(arr_ptr) > 0) {
                int tmp;
                dynarr_pop(int)(arr_ptr, &tmp);
            }
        } else {
            // Create a new array and insert into list
            dynarr_int new_arr;
            dynarr_init(int)(&new_arr, 2);
            list_push_after(int)(&list_of_arrays, NULL, new_arr);
        }
    }

    // Remove empty arrays (will automatically destroy dynarr via D1)
    list_node_int* cur = list_first(int)(&list_of_arrays);
    while (cur) {
        list_node_int* next = list_next(int)(cur);
        dynarr_int* arr_ptr = list_access(int)(cur);

        if (dynarr_size(int)(arr_ptr) == 0) {
            list_pop(int)(&list_of_arrays, cur, NULL);
        }

        cur = next;
    }

    // Print remaining arrays
    size_t arr_idx = 0;
    for (list_node_int* n = list_first(int)(&list_of_arrays); n != NULL; n = list_next(int)(n)) {
        dynarr_int* arr_ptr = list_access(int)(n);
        printf("Array %zu: ", arr_idx++);
        for (size_t i = 0; i < dynarr_size(int)(arr_ptr); i++) {
            printf("%d ", dynarr_access(int)(arr_ptr)[i]);
        }
        printf("\n");
    }

    // Cleanup: remaining arrays are destroyed automatically via list_destroy
    list_destroy(int)(&list_of_arrays);

    return 0;
}
