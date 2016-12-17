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

// Cortex-A7 cache line is 64 bytes
// Since malloc'ed memory should be cache-aligned and yet have overhead bytes
// the smallest block we can allocate is 2*cache line size
//
#define CACHE_LINE_POWER 6
#define CACHE_LINE_SIZE  (1<<CACHE_LINE_POWER)
#define MIN_BLOCK_POWER  7
#define MIN_BLOCK_SIZE   (1<<MIN_BLOCK_POWER)



#define MAGIC 0xFEEFBEEF
#define BLOCK_FREE 0x01

typedef struct block_header {
    uint32_t            magic;
    uint32_t            flags;
    uint32_t            size;
    struct block_header *next;
    // must be <= CACHE_LINE_SIZE!!
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


#define FREE_SLOTS (sizeof(void *) * 8)
static block_header_t *heap[FREE_SLOTS];

static inline block_header_t *buddy(block_header_t *block)
{
    return (block_header_t *)((uintptr_t)block ^= block->size);
}

static block_header_t *split_block(block_header_t *big)
{
    int slot = (big->size >> 1);
    big->size = slot;
    block_header_t *buddy = buddy(big);

    big->next = heap[slot];
    heap[slot] = big;

    buddy->magic = MAGIC;
    buddy->size = slot;
    buddy->flags = BLOCK_FREE;
    buddy->next = NULL;
    return buddy;
}


void heap_init(void *start, size_t length)
{
    // let's assume *start is cache aligned and length is a multiple of
    // MIN_BLOCK_SIZE

    index = high_bit(length);
    heap[index] = (block_header_t *)start;
    heap[index]->magic = MAGIC;
    heap[index]->flags = BLOCK_FREE;
    heap[index]->size = length;
    heap[index]->next = NULL;
}

void *malloc(size_t length)
{
    int slot;
    if (length == 0) return NULL;
    length += CACHE_LINE_SIZE;   // for the malloc header
    int bit = high_bit(length);
    // round up to the next power of two:
    if (length != (1<<bit)) {
        length += ((1<<bit) - 1);
        length &= ~((1<<bit) - 1);
        if (length > (1<<bit))
            bit += 1;
    }

    if (heap[bit]) {
        void *ptr = heap[bit];
        heap[bit] = heap[bit]->next;
        return ((uintptr_t)ptr + CACHE_LINE_SIZE);
    }

    // find next largest free block
    int slot = bit + 1;
    while (slot < FREE_SLOTS && !heap[slot])
        ++slot;

    // TODO: assert here
    if (slot == FREE_SLOTS) return NULL;

    block_header_ptr *fb = heap[slot];
    heap[slot] = fb->next;

    while (slot > bit) {
        fb = split_block(fb);
        --slot;
    }

    return ((uintptr_t)fb + CACHE_LINE_SIZE);
}


void free(void *ptr)
{
    block = (block_header_ptr *)ptr;
    if (block) {
        // assert(block->magic == MAGIC);
        block->flags = BLOCK_FREE;
        block_header_ptr *buddy = buddy(block);
        while (buddy->flags == BLOCK_FREE
               && buddy->size == block->size) {
            // remove buddy from it's free list
            block->size <<= 1;
            buddy = buddy(block);
        }
        block->next = heap[block->size];
        heap[block->size] = block;
    }
}



