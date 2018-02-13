
/*
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file balloc.c
 *
 * memory allocation services
 *
 * Functions are exported by os.h
 *
 */

#include <stdint.h>

#define NULL 0
/******* Framework headers : */
//#include "os/os.h"          /* framework export definitions */
//#include "os/os_types.h"    /* framework-specific types */
//#include "zephyr/common.h"
//#include "zephyr/os_config.h"
//#include "infra/log.h"

#if defined(MALLOC_ENABLE_STATISTICS)
#include "zephyr/os_specific.h"   /* need _log function */
#endif



typedef  unsigned short int uint16_t;
typedef  unsigned int uint32_t;

/**********************************************************
 ************** Extern variables   ************************
 **********************************************************/




/**********************************************************
 ************** Local definitions  ************************
 **********************************************************/
#define BITS_PER_UINT32 (sizeof(uint32_t) * 8)

/** Descriptor for a memory pool */
typedef struct {
    uint32_t *track; /** block allocation tracker */
    uint32_t start; /** start address of the pool */
    uint32_t end;   /** end address of the pool */
    uint16_t count; /** total number of blocks within the pool */
    uint16_t size;  /** size of each memory block within the pool */
#if defined(MALLOC_ENABLE_STATISTICS)
    uint32_t max;  /** maximum number of allocated blocks at the same time */
    uint32_t cur;  /** current number of allocated blocks */
#endif
}T_POOL_DESC;


/**********************************************************
 ************** Private variables  ************************
 **********************************************************/

#if defined(MALLOC_ENABLE_STATISTICS)

/** Allocate the memory blocks and tracking variables for each pool */
#define DECLARE_MEMORY_POOL(index, size, count)	\
    uint8_t g_MemBlock_ ## index[count][size]; \
uint32_t g_MemBlock_alloc_track_ ## index[count / BITS_PER_UINT32 + \
1] = { 0 };

#include "memory_pool_list.def"


/** Pool descriptor definition */
T_POOL_DESC g_MemPool[] =
{
#define DECLARE_MEMORY_POOL(index, size, count)	\
    { \
        /* T_POOL_DESC.track */ g_MemBlock_alloc_track_ ## index, \
        /* T_POOL_DESC.start */ (uint32_t)g_MemBlock_ ## index,	\
        /* T_POOL_DESC.end */ (uint32_t)g_MemBlock_ ## index + count * size, \
        /* T_POOL_DESC.count */ count, \
        /* T_POOL_DESC.size */ size, \
        /* T_POOL_DESC.max */ 0, \
        /* T_POOL_DESC.cur */ 0	\
    },

#include "memory_pool_list.def"
};


#else

/** Allocate the memory blocks and tracking variables for each pool */
#define DECLARE_MEMORY_POOL(index, size, count)	\
    uint8_t g_MemBlock_ ## index[count][size]; \
uint32_t g_MemBlock_alloc_track_ ## index[count / BITS_PER_UINT32 + \
1] = { 0 }; \

#include "memory_pool_list.def"



/** Pool descriptor definition */
T_POOL_DESC g_MemPool [] =
{
#define DECLARE_MEMORY_POOL(index, size, count)	\
    { \
        /* T_POOL_DESC.track */ g_MemBlock_alloc_track_ ## index, \
        /* T_POOL_DESC.start */ (uint32_t)g_MemBlock_ ## index,	\
        /* T_POOL_DESC.end */ (uint32_t)g_MemBlock_ ## index + count * size, \
        /* T_POOL_DESC.count */ count, \
        /* T_POOL_DESC.size */ size \
    },

#include "memory_pool_list.def"
};



#endif


/** Number of memory pools */
#define NB_MEMORY_POOLS   (sizeof(g_MemPool) / sizeof(T_POOL_DESC))

/**********************************************************
 ************** Private functions  ************************
 **********************************************************/

/**
 * \brief  Return the next free block of a pool and
 *   mark it as reserved/allocated
 *
 * \param pool index of the pool in g_MemPool
 *
 * \return allocated buffer or NULL if none is
 *   available
 */
static void *memblock_alloc(uint32_t pool)
{
    uint16_t block;

    for (block = 0; block < g_MemPool[pool].count; block++) {
        if (((g_MemPool[pool].track)[block /
                    BITS_PER_UINT32] & 1 <<
                    (BITS_PER_UINT32 - 1 - (block % BITS_PER_UINT32))) == 0) {
            (g_MemPool[pool].track)[block /
                BITS_PER_UINT32] =
                (g_MemPool[pool].track)[block /
                BITS_PER_UINT32] |
                (1 <<
                 (BITS_PER_UINT32 - 1 -
                  (block % BITS_PER_UINT32)));
#if defined(MALLOC_ENABLE_STATISTICS)
            g_MemPool[pool].cur = g_MemPool[pool].cur + 1;
            if (g_MemPool[pool].cur > g_MemPool[pool].max)
                g_MemPool[pool].max = g_MemPool[pool].cur;
#endif
            return (void *)(g_MemPool[pool].start +
                    g_MemPool[pool].size * block);
        }
    }
    return NULL;
}



/**
 * \brief Free an allocated block from a pool
 *
 * \param pool index of the pool in g_MemPool
 *
 * \param ptr points to the start of the block
 *     to free
 *
 */
static void memblock_free(uint32_t pool, void *ptr)
{
    uint16_t block;

    block = ((uint32_t)ptr - g_MemPool[pool].start) / g_MemPool[pool].size;
    if (block < g_MemPool[pool].count) {
        (g_MemPool[pool].track)[block /
            BITS_PER_UINT32] &=
            ~(1 << (BITS_PER_UINT32 - 1 - (block % BITS_PER_UINT32)));
#if defined(MALLOC_ENABLE_STATISTICS)
        g_MemPool[pool].cur = g_MemPool[pool].cur - 1;
#endif
    }
#if defined(__DEBUG_OS_ABSTRACTION_BALLOC)
    else
        _log(
                "ERR: memblock_free: ptr 0x%X is not within pool %d [0x%X , 0x%X]",
                ptr, pool, g_MemPool[pool].start, g_MemPool[pool].end);
#endif
}




/**
 * \brief Test if a block is allocated
 *
 * \param pool index of the pool in g_MemPool
 *
 * \param ptr points to the start of the block
 *
 * \return 1 if the block is allocated/reserved,
 *   0 if the block is free
 *
 */
static unsigned char memblock_used(uint32_t pool, void *ptr)
{
    uint16_t block;

    block = ((uint32_t)ptr - g_MemPool[pool].start) / g_MemPool[pool].size;
    if (block < g_MemPool[pool].count) {
        if (((g_MemPool[pool].track)[block /
                    BITS_PER_UINT32] &
                    (1 <<
                     (BITS_PER_UINT32 - 1 - (block % BITS_PER_UINT32)))) !=
                0) {
            return 1;
        }
    }
    return 0;
}


int main()
{
    uint16_t *a = balloc(8, NULL);
    return 0;
} 
