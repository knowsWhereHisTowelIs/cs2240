/**
* Who: Caleb Slater
* What: A4
* When: 2015-10-01
* Where: CS 2240
* How: "$./a.out"
* Memory play
**/

#include "nu_malloc.h"

void *globalBaseBlock = NULL;

/**
  * Searches through blocks and finds the next availble block
  * @param blockHeaderPtr *last
  * @param size_t size
**/
blockHeaderPtr findFreeBlock(blockHeaderPtr *last, size_t size) {
    //create temp var of current free block
    blockHeaderPtr current = globalBaseBlock;
    //search until we find a block that is free or the current block is bigger than the requested block
    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

/**
  * Add more space in memory, return pointer to new space
  * @param blockHeaderPtr last
  * @param size_t size
  * @return blockHeaderPtr block
**/
blockHeaderPtr requestSpace(blockHeaderPtr last, size_t size) {
    blockHeaderPtr block;
    block = sbrk(0); //current block of memory
    void *request = sbrk(size + BLOCK_HEADER_SIZE); //request more memory

    assert((void*) block == request); // Not thread safe.
    if (request == (void*) -1) {
        return NULL; // sbrk failed.
    }

    if (last) {
        // NULL on first request.
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;

    return block;
}

/**
  * get pointer to block
  * @param void *ptr
**/
blockHeaderPtr getBlockPtr(void *ptr) {
    return (blockHeaderPtr) ptr - 1;
}

/**
  * allocate a block in memory of requested size
  * @param size_t size requested size
  * @return pointer to new block or null on failure
**/
void *nu_malloc(size_t size) {
    blockHeaderPtr block;

    if (size <= 0) { //invalid size
        return NULL;
    }

    if (!globalBaseBlock) {
        // First call of nu_malloc
        block = requestSpace(NULL, size);
        if (!block) {
            //error
            return NULL;
        }
        //set global to current block
        globalBaseBlock = block;
    } else {
        blockHeaderPtr lastBlock = globalBaseBlock;
        block = findFreeBlock(&lastBlock, size);
        if (!block) {
            // Failed to find free block.
            // so create new block
            block = requestSpace(lastBlock, size);
            if (!block) {
                // error
                return NULL;
            }
        } else {
            // Found free block
            block->free = 0;
        }
    }

    return (block+1);
}

/**
  * Delete pointer in memory
  * @param blockHeaderPtr last
  * @return void
**/
void nu_free(void *ptr) {
    if (!ptr) { // NULL pointer so don't free and return
        return;
    }

    // TODO: consider merging blocks once splitting blocks is implemented.
    blockHeaderPtr  blockPtr = getBlockPtr(ptr);
    assert(blockPtr->free == 0);
    blockPtr->free = 1;
}

/**
  * reallocate memory for pointer
  * @param void *ptr
  * @param size_t size
  * @return pointers address after reallocation
**/
void *nu_realloc(void *ptr, size_t size) {
    if (!ptr) {
        // NULL ptr. realloc should act like malloc.
        return nu_malloc(size);
    }

    blockHeaderPtr  blockPtr = getBlockPtr(ptr);
    if (blockPtr->size >= size) {
        // We have enough space. Could free some once we implement split.
        return ptr;
    } else {
        // Need to really realloc. Malloc new space and free old space.
        // Then copy old data to new space.
        void *newPtr;
        newPtr = nu_malloc(size);
        if (!newPtr) {
            // NULL pointer error
            return NULL; // TODO: set errno on failure.
        }
        memcpy(newPtr, ptr, blockPtr->size);
        nu_free(ptr); //delete old reference
        return newPtr;
    }
}

/**
  * Allocate memory and initialize it to zero
  * @param size_t numberOfElements
  * @param size_t elementSize
  * @return ptr
**/
void *nu_calloc(size_t numberOfElements, size_t elementSize) {
    size_t size = numberOfElements * elementSize; // TODO: check for overflow.
    void *ptr = nu_malloc(size);
    memset(ptr, 0, size);
    return ptr;
}
