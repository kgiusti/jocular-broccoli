/*
 *    Copyright 2016, Kenneth A Giusti
 *
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may
 *    not use this file except in compliance with the License. You may obtain
 *    a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *    License for the specific language governing permissions and limitations
 *    under the License.
 */

#include <types.h>
#include <llist.h>


// Cortex-A7 type alignment
#define MALLOC_ALIGNMENT        8

// Cortex-A7 cache line is 64 bytes
#define CACHE_LINE_POWER 6
#define CACHE_LINE_SIZE  (1<<CACHE_LINE_POWER)



#define MAGIC_FREE 0xFEEFBEEB
#define MAGIC_USED 0xEFFEEBBE
#define BLOCK_MIN_SIZE  64

typedef struct block_header {
    uint32_t            magic;  // 0..3
    uint32_t            slot;   // 4..7
#define HEADER_OFFSET 8
    LIST_LINKAGE(struct block_header, list);
} block_header_t;


static inline int high_bit(size_t length)
{
    uint_t bit = 0;
    while (length) {
        length >>= 1;
        bit += 1;
    }
    return bit - 1;
}

DEFINE_LIST(block_header_t, block);

#define FREE_SLOTS (sizeof(void *) * 8)
static block_header_list_t heap[FREE_SLOTS];

static inline block_header_t *buddy(block_header_t *block)
{
    return (block_header_t *)((uintptr_t)block ^= block->slot);
}

static inline block_header_t *low_buddy(block_header_t *block)
{
    return (block_header_t *)((uintptr_t)block &= ~block->slot);
}

static void split_block(block_header_t *block)
{
    block->slot >>= 1;
    block_header_t *buddy = buddy(block);

    buddy->magic = MAGIC_FREE;
    buddy->slot = block->slot;
    LINK_INIT(buddy, list);
    PUSH_TAIL(&heap[buddy->slot], buddy, list);
}

static block_header_t *merge_block(block_header_t *block)
{
    int slot = block->slot;
    block_header_t *buddy = buddy(block);
    if (buddy->magic == MAGIC_FREE && buddy->slot == slot) {
        LIST_REMOVE(&heap[slot], buddy, list);
        block = low_buddy(block);
        block->slot <<= 1;
        buddy(block)->magic = 0;
        return merge_block(block);
    }
    return block;
}


void heap_init(void *start, size_t length)
{
    // let's assume *start is cache aligned and length is a multiple of 2

    for (int i = 0; i < FREE_SLOTS; ++i)
        LIST_INIT(&heap[i]);

    slot = high_bit(length);
    block_header_t *block = (block_header_t *start);
    block->magic = MAGIC_FREE;
    block->slot = slot;
    PUSH_TAIL(&heap[slot], block, list);
}


void *malloc(size_t length)
{
    if (length == 0) return NULL;
    length += HEADER_OFFSET;
    if (length < BLOCK_MIN_SIZE)
        length = BLOCK_MIN_SIZE;

    // round up to the next power of two:
    int slot = high_bit(length);
    if (length > (1<<slot)) {
        slot += 1;
    }

    int index = slot;
    while (index < FREE_SLOTS && heap[index].count == 0)
        ++index;

    // TODO: assert here
    if (index == FREE_SLOTS) return NULL;

    block_header_ptr *block;
    POP_HEAD(&heap[index], block, list);

    while (block->slot > slot) {
        block = split_block(block);
    }

    block->magic = MAGIC_USED;
    return (void *)((uintptr_t *)block + HEADER_OFFSET);
}


void free(void *ptr)
{
    if (ptr) {
        block_header_ptr *block;
        block = (block_header_ptr *)((uintptr_t)ptr - HEADER_OFFSET);
        // assert(block->magic == MAGIC_USED);
        block->magic = MAGIC_FREE;
        block = merge_block(block);
        PUSH_TAIL(&heap[block->slot], block, list);
    }
}


// aligned on a cache line
void *malloc_aligned(size_t length)
{
    return NULL;
}

