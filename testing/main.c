#include <stdlib.h>

#define SU int
#define T1 int
#include "cup/dynarr.h"
#include "cup/undef.h"

#define SU flo
#define T1 float
#include "cup/dynarr.h"
#include "cup/undef.h"

void des(void* tar) {
    dynarr_destroy(flo, tar);
}

#define  SU 2d
#define  T1 dynarr_flo
#define  D1 des
#include "cup/dynarr.h"
#include "cup/undef.h"

int main() {
    dynarr_flo arrf;
    dynarr_init(flo, &arrf, 128);

    dynarr_2d arr2;
    dynarr_init(2d, &arr2, 128);
    dynarr_push(2d, &arr2, arrf); // transfer, arrf no longer owned here
    dynarr_pop(2d, &arr2, NULL);

    dynarr_destroy(2d, &arr2);
    return 0;
}
