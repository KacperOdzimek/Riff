#pragma once

#define CAT2(a, b) a##b
#define CAT(a, b)  CAT2(a, b)

// for making generic struct names
#define NAME_MAKE(base, suf)      CAT(base, suf)

// for making type specific functions implementations
#define FUNC_IMPL(func_name)      CAT(CAT(func_name, _implementation_for_), SU)

// for making dispatch macros
#define FUNC_RESP(func_name, LSU) CAT(CAT(func_name, _implementation_for_), LSU)

#define SCC 1
#define ERR 0

// default allocator
#ifndef A
    #define A malloc
#endif
