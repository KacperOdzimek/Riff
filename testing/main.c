#include <stdlib.h>

#define SU int
#define T1 int
#include "cup/dynarr.h"
#include "cup/undef.h"

#define SU flo
#define T1 float
#include "cup/dynarr.h"
#include "cup/undef.h"

int main() {
    dynarr_int arri;
    dynarr_init(int, &arri, 128);

    

    dynarr_destroy(int, &arri);
    return 0;
}
