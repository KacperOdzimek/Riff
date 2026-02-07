#include <stdlib.h>
#include <stdio.h>

size_t hash(const int* x) {return (size_t)(*x); };
int equal(int a, int b) {return a == b; };

#define SU int_int
#define T1 int
#define T2 int
#define F1 hash
#define F2 equal
#include "cup/hashmap.h"

int main() {

}
