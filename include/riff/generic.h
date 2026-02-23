#ifndef GENERIC_H
#define GENERIC_H

// T/A macros selectors
#define RIFF_FIRST_IMPL(a, ...) a
#define RIFF_FIRST(...)   RIFF_FIRST_IMPL(__VA_ARGS__)

#define RIFF_SECOND_IMPL(a,b, ...) b
#define RIFF_SECOND(...)  RIFF_SECOND_IMPL(__VA_ARGS__)

#define RIFF_THIRD_IMPL(a,b,c, ...) c
#define RIFF_THIRD(...)   RIFF_THIRD_IMPL(__VA_ARGS__)

#define RIFF_FOURTH_IMPL(a,b,c,d, ...) d
#define RIFF_FOURTH(...)  RIFF_FOURTH_IMPL(__VA_ARGS__)

#define RIFF_FIFTH_IMPL(a,b,c,d,e, ...) e
#define RIFF_FIFTH(...)   RIFF_FIFTH_IMPL(__VA_ARGS__)

#define RIFF_SIXTH_IMPL(a,b,c,d,e,f, ...) f
#define RIFF_SIXTH(...)   RIFF_SIXTH_IMPL(__VA_ARGS__)

#define RIFF_SEVENTH_IMPL(a,b,c,d,e,f,g, ...) g
#define RIFF_SEVENTH(...) RIFF_SEVENTH_IMPL(__VA_ARGS__)

#define RIFF_EIGHTH_IMPL(a,b,c,d,e,f,g,h, ...) h
#define RIFF_EIGHTH(...)  RIFF_EIGHTH_IMPL(__VA_ARGS__)

#define RIFF_NINTH_IMPL(a,b,c,d,e,f,g,h,i, ...) i
#define RIFF_NINTH(...)   RIFF_NINTH_IMPL(__VA_ARGS__)

#define RIFF_TENTH_IMPL(a,b,c,d,e,f,g,h,i,j, ...) j
#define RIFF_TENTH(...)   RIFF_TENTH_IMPL(__VA_ARGS__)

// fir joining texts together
#define RIFF_CAT_IMPL(a, b) a##b
#define RIFF_CAT(a, b)  RIFF_CAT_IMPL(a, b)

// for making instances-names
#define RIFF_INST(thing, instance_name) RIFF_CAT(RIFF_CAT(riff_implementation_of_, thing), RIFF_CAT(_for_, instance_name))

// for defining functions
#define RIFF_API(ret) static inline ret

// success flag
#define SCC 1

// error flag
#define ERR 0

// second layer, just to ensure correct expansion
#define RIFF_PASS_VAARGS(...) __VA_ARGS__

// dispatch A macro
#define RIFF_ALLOC   RIFF_FIRST(RIFF_PASS_VAARGS(A))
#define RIFF_REALLOC RIFF_SECOND(RIFF_PASS_VAARGS(A))
#define RIFF_FREE    RIFF_THIRD(RIFF_PASS_VAARGS(A))

#endif // GENERIC_H
