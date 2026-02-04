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
    list_int list;
    list_init(int)(&list);

    

    for (list_node_int* n = list_first(int)(&list); n != NULL; n = list_next(int)(n)) {
        printf("%d\n", *list_const_access(int)(n));
    }

    list_destroy(int)(&list);
}
