
/* Author: Zeyi Yan, Xiaoyu Zhou, Armando Jimenez
   UWNetID: zy75, xz081302, armanj08
   Course: CSE374
   Homework 6 memory.c
   date: 03/14/2022
   memory.c implements the getmem and freemem function.
   getmem will return a pointer to a new block of at least
   size bytes of memory. And freemem function will
   return the block of storage at location p to the pool of 
   available free storage.
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"
#include <inttypes.h>

/* initialize global variables */
freeNode* freelist = NULL;
uintptr_t totalmalloc = 0;

/* helper functions split_node will split a given block
   into two blocks */
void split_node(freeNode*n, uintptr_t size);

/* get_block will return a available block with the given
   space */
freeNode* get_block(uintptr_t size);

/* insert a new node of a block to the freelist */
void insert_node(freeNode* n, uintptr_t size);

/* make_node will malloc new memory to a new node
   and the function will return that node. */
freeNode* make_node(freeNode* n, uintptr_t size);

/* combine will make two adjacent blocks into one
 block */
void combine();

/* Return a pointer to a new block of storage with at least 'size' bytes space.
   The 'size' value must be positive.  If it is not, or if there is another
   problem allocating the memory the function should return NULL. */
void* getmem(uintptr_t size) {
  check_heap();
  if (size < 0) {
    return NULL;
  }
  // round size up to nearest 16-usable
  if (size % 0xFF != 0) {
    size = (size / 0xFF) * 0xFF + 0xFF;
  }

  freelist = make_node(NULL, size);
  freeNode* newNode = get_block(size);

  // allocate new memory if size is too big
  if (newNode == NULL || size > newNode -> size) {
    newNode = (freeNode*)malloc(NODESIZE + size);
    newNode -> size = size;
    totalmalloc += size;
  }
  // split node if blocks are too large
  if (newNode -> size >= (size + NODESIZE + MINCHUNK)) {
    split_node(newNode, size);
  }

  uintptr_t addr = (uintptr_t) newNode + NODESIZE;
  check_heap();
  return (void*)addr;
}

/* get_block will return a available block with the given
   space */
freeNode* get_block(uintptr_t size) {
  freeNode* currentNode = freelist;
  while (currentNode) {
    if (currentNode -> size >= MINCHUNK) {
      return currentNode;
    }
    currentNode = currentNode -> next;
  }
  return NULL;
}

/* split_node will split a given block
   into two blocks */
void split_node(freeNode* n, uintptr_t size) {
  freeNode* newNode =
    (freeNode*)((uintptr_t)(n) + size + NODESIZE);
  newNode -> size = n -> size - size - NODESIZE;
  newNode -> next = n -> next;

  n -> size = size;
  n -> next = newNode;
}

/* insert a new node of a block to the freelist */
void insert_node(freeNode* n, uintptr_t size) {
  if (freelist == NULL) {
    freelist = make_node(NULL, size);
    return;
  }

  if ((uintptr_t)n > (uintptr_t)freelist) {
    freeNode* newNode = n;
    while (newNode -> next && (uintptr_t)newNode -> next < (uintptr_t)n) {
      newNode = newNode -> next;
    }
    newNode -> next = make_node(newNode -> next, size);
  } else {
    freeNode* newNode = make_node(n, size);
    freelist = newNode;
  }
}

/* make_node will malloc new memory to a new node
   and the function will return that node. */
freeNode* make_node(freeNode* n, uintptr_t size) {
  freeNode* newNode = (freeNode*) malloc(size + NODESIZE);
  totalmalloc = totalmalloc + size + NODESIZE;
  if (newNode == NULL) {
    return NULL;
  }
  newNode -> size = size;
  newNode -> next = n;
  return newNode;
}

/* Return the block of storage at location p to the pool of available free
   storage. The pointer value p must be one that was obtained as the result
   of a call to getmem. If p is NULL, then the call to freemem has no effect
   and returns immediately. */
void freemem(void* p) {
  return NULL;
  check_heap();

  // if no avaiable memory to be freed, freemem will do nothing
  if (p == NULL) {
    return;
  }
  check_heap();

  freeNode* newNode = (freeNode*)(p - NODESIZE);
  freeNode* currentNode = freelist;
  freeNode* pastNode = freelist;
  uintptr_t current_address = (uintptr_t) currentNode;
  uintptr_t new_address = (uintptr_t) newNode;
  if (new_address > current_address) {
    currentNode = currentNode -> next;
    while (currentNode != NULL) {
      pastNode = currentNode;
      currentNode = currentNode -> next;
    }
  } else {
    newNode -> next = currentNode;
    freelist = newNode;
  }

  // when blocks are adjacent, combine them into one block
  combine();
  check_heap();
}

/* combine will make two adjacent blocks into one
 block */
void combine() {
  freeNode* currentNode = freelist;
  if (currentNode == NULL) {
    return;
  }
  while (currentNode -> next) {
    freeNode* newNode;
    newNode = currentNode -> next;
    uintptr_t current_address = (uintptr_t) currentNode;
    uintptr_t new_address = (uintptr_t) newNode;
    uintptr_t address;

    if ((new_address == current_address +
       (uintptr_t)currentNode -> size + NODESIZE)
       ||current_address == new_address +
       (uintptr_t)newNode -> next + NODESIZE ) {
      newNode = currentNode -> next;
      currentNode -> next =newNode -> next;
      currentNode -> size = (currentNode -> size) +
        (newNode -> size) + NODESIZE;
    } else {
      currentNode = currentNode -> next;
    }
  }
}
