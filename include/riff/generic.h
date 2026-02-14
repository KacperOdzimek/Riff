#ifndef GENERIC_H
#define GENERIC_H

#define CAT2(a, b) a##b
#define CAT(a, b)  CAT2(a, b)

// for making generic struct names
#define NAME(base, suf)      CAT(CAT(CAT(riff_struct_of_, base), _), suf)

// for making dispatch macros
#define IMPL(func_name, SUF) CAT(CAT(riff_implementation_of_, func_name), CAT(_for_, SUF))

// for making internal functions
#define INTR(func_name, SUF) CAT(CAT(riff_internal_func_, func_name), CAT(_for_, SUF))

// success flag
#define SCC 1

// error flag
#define ERR 0

#endif // GENERIC_H

// do on every include:

// default allocator
#ifndef A
    #define A malloc
#endif

#ifndef R
    #define R realloc
#endif

#ifndef F
    #define F free
#endif
