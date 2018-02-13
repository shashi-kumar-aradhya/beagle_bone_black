# 1 "balloc.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "balloc.c"
# 41 "balloc.c"
# 1 "/usr/lib/gcc/x86_64-linux-gnu/5/include/stdint.h" 1 3 4
# 9 "/usr/lib/gcc/x86_64-linux-gnu/5/include/stdint.h" 3 4
# 1 "/usr/include/stdint.h" 1 3 4
# 25 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 367 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 410 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 411 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 368 "/usr/include/features.h" 2 3 4
# 391 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 392 "/usr/include/features.h" 2 3 4
# 26 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wchar.h" 1 3 4
# 27 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/stdint.h" 2 3 4
# 36 "/usr/include/stdint.h" 3 4

# 36 "/usr/include/stdint.h" 3 4
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;

typedef long int int64_t;







typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

typedef unsigned int uint32_t;



typedef unsigned long int uint64_t;
# 65 "/usr/include/stdint.h" 3 4
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;

typedef long int int_least64_t;






typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;

typedef unsigned long int uint_least64_t;
# 90 "/usr/include/stdint.h" 3 4
typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
# 103 "/usr/include/stdint.h" 3 4
typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
# 119 "/usr/include/stdint.h" 3 4
typedef long int intptr_t;


typedef unsigned long int uintptr_t;
# 134 "/usr/include/stdint.h" 3 4
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
# 10 "/usr/lib/gcc/x86_64-linux-gnu/5/include/stdint.h" 2 3 4
# 42 "balloc.c" 2
# 57 "balloc.c"

# 57 "balloc.c"
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
# 73 "balloc.c"
typedef struct {
    uint32_t *track;
    uint32_t start;
    uint32_t end;
    uint16_t count;
    uint16_t size;




}T_POOL_DESC;
# 127 "balloc.c"
# 1 "memory_pool_list.def" 1
# 13 "memory_pool_list.def"
uint8_t g_MemBlock_0[32][8]; uint32_t g_MemBlock_alloc_track_0[32 / (sizeof(uint32_t) * 8) + 1] = { 0 };
# 128 "balloc.c" 2




T_POOL_DESC g_MemPool [] =
{
# 143 "balloc.c"
# 1 "memory_pool_list.def" 1
# 13 "memory_pool_list.def"
{ g_MemBlock_alloc_track_0, (uint32_t)g_MemBlock_0, (uint32_t)g_MemBlock_0 + 32 * 8, 32, 8 },
# 144 "balloc.c" 2
};
# 167 "balloc.c"
static void *memblock_alloc(uint32_t pool)
{
    uint16_t block;

    for (block = 0; block < g_MemPool[pool].count; block++) {
        if (((g_MemPool[pool].track)[block /
                    (sizeof(uint32_t) * 8)] & 1 <<
                    ((sizeof(uint32_t) * 8) - 1 - (block % (sizeof(uint32_t) * 8)))) == 0) {
            (g_MemPool[pool].track)[block /
                (sizeof(uint32_t) * 8)] =
                (g_MemPool[pool].track)[block /
                (sizeof(uint32_t) * 8)] |
                (1 <<
                 ((sizeof(uint32_t) * 8) - 1 -
                  (block % (sizeof(uint32_t) * 8))));





            return (void *)(g_MemPool[pool].start +
                    g_MemPool[pool].size * block);
        }
    }
    return 0;
}
# 205 "balloc.c"
static void memblock_free(uint32_t pool, void *ptr)
{
    uint16_t block;

    block = ((uint32_t)ptr - g_MemPool[pool].start) / g_MemPool[pool].size;
    if (block < g_MemPool[pool].count) {
        (g_MemPool[pool].track)[block /
            (sizeof(uint32_t) * 8)] &=
            ~(1 << ((sizeof(uint32_t) * 8) - 1 - (block % (sizeof(uint32_t) * 8))));



    }






}
# 240 "balloc.c"
static unsigned char memblock_used(uint32_t pool, void *ptr)
{
    uint16_t block;

    block = ((uint32_t)ptr - g_MemPool[pool].start) / g_MemPool[pool].size;
    if (block < g_MemPool[pool].count) {
        if (((g_MemPool[pool].track)[block /
                    (sizeof(uint32_t) * 8)] &
                    (1 <<
                     ((sizeof(uint32_t) * 8) - 1 - (block % (sizeof(uint32_t) * 8))))) !=
                0) {
            return 1;
        }
    }
    return 0;
}


int main()
{
    uint16_t *a = balloc(8, 0);
    return 0;
}
