#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

/* ---- dynarr instantiation ---- */

#define SU int
#define T1 int
#include "cup/dynarr.h"
#include "cup/undef.h"

/* ---- helpers ---- */

static size_t rand_size(size_t max) {
    if (max == 0) return 0;
    return (size_t)(rand() % (int)max);
}

static int rand_bool(void) {
    return rand() & 1;
}

/* ---- test logic ---- */

#define ARR_COUNT 8
#define OPS_COUNT 200000

int main(void) {
    srand((unsigned)time(NULL));

    dynarr_int arrays[ARR_COUNT];
    int alive[ARR_COUNT] = {0};

    /* random initial state */
    for (int i = 0; i < ARR_COUNT; i++) {
        if (rand_bool()) {
            size_t cap = rand_size(16);
            if (dynarr_init(int)(arrays + i, cap) == SCC) {
                alive[i] = 1;
            }
        }
    }

    for (int step = 0; step < OPS_COUNT; step++) {
        int i = rand() % ARR_COUNT;
        int op = rand() % 7;

        switch (op) {
        case 0: /* init */
            if (!alive[i]) {
                size_t cap = rand_size(32);
                if (dynarr_init(int)(arrays + i, cap) == SCC) {
                    alive[i] = 1;
                }
            }
            break;

        case 1: /* destroy */
            if (alive[i]) {
                dynarr_destroy(int)(arrays + i);
                alive[i] = 0;
            }
            break;

        case 2: /* push */
            if (alive[i]) {
                int val = rand();
                dynarr_push(int)(arrays + i, val);
            }
            break;

        case 3: /* pop */
            if (alive[i]) {
                if (rand_bool()) {
                    int out;
                    dynarr_pop(int)(arrays + i, &out);
                } else {
                    dynarr_pop(int)(arrays + i, NULL);
                }
            }
            break;

        case 4: /* reserve */
            if (alive[i]) {
                size_t cap = rand_size(128);
                dynarr_reserve(int)(arrays + i, cap);
            }
            break;

        case 5: /* shrink_to_fit */
            if (alive[i]) {
                dynarr_shrink_to_fit(int)(arrays + i);
            }
            break;

        case 6: /* raw access mutation */
            if (alive[i]) {
                int *data = dynarr_access(int)(arrays + i);
                size_t n = rand_size(8);

                for (size_t j = 0; j < n && j < arrays[i].priv_size; j++) {
                    data[j] ^= 0x55AA55AA;
                }
            }
            break;
        }
    }

    /* final cleanup */
    for (int i = 0; i < ARR_COUNT; i++) {
        if (alive[i]) {
            dynarr_destroy(int)(arrays + i);
        }
    }

    puts("dynarr random test finished");
    return 0;
}
