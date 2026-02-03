#include <stdlib.h>

#define SU flo
#define T1 float
#include "cup/dynarr.h"
#include "cup/undef.h"

#define  SU 2d
#define  T1 dynarr_flo
#define  D1 dynarr_destroy(flo)
#include "cup/dynarr.h"
#include "cup/undef.h"

int main() {
    dynarr_flo arrf;
    dynarr_init(flo)(&arrf, 128);

    dynarr_2d arr2;
    dynarr_init(2d)(&arr2, 128);
    dynarr_push(2d)(&arr2, arrf); // transfer, arrf no longer owned here
    dynarr_pop(2d)(&arr2, NULL);

    dynarr_destroy(2d)(&arr2);
    return 0;
}
