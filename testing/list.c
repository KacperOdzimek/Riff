#include <stdlib.h>
#include <stdio.h>

void dest(int* ptr) {
    int x = *ptr;
    printf("Destructed: %d", x);
}

#define SU int
#define T1 int
#define D1 dest
#include "cup/list.h"
#include "cup/undef.h"

int main() {
    list_int* list;
    list_init(int)(list);

    

    list_destroy(int)(list);
}
