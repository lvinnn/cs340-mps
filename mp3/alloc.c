/**
 * Malloc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _metadata_t {
  unsigned int size;     // The size of the memory block.
  unsigned char isUsed;  // 0 if the block is free; 1 if the block is used.
  void* nextBlock;
  void* nextFree;
} metadata_t;

void* startOfHeap = NULL;
metadata_t* lastBlock = NULL;
void* startOfFree = NULL;

void printHeap() {
  if(startOfHeap != NULL) {
    metadata_t *curr = startOfHeap;
    while(curr) {
      printf("%d,%d ", curr->size, curr->isUsed);
      curr = curr->nextBlock;
    }
    printf("\n");
  }
}

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
  // implement calloc:
  void *ptr = malloc(num * size);
  if(ptr == NULL) {
    return NULL;
  }
  char* data = ptr;
  for(int i=0; i<num*size; i++) {
    data[i] = 0x00;
  }

  // printHeap();
  return ptr;
}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
  // implement malloc:
  // Allocate heap memory for the metadata structure and populate the variables:

  if (startOfHeap == NULL) {
    startOfHeap = sbrk(0);
  } else {
    metadata_t* curr = startOfFree;
    metadata_t* prevFree = startOfFree;
    
    while(curr != NULL) {
      if(curr->size >= size) {
        curr->isUsed = 1;
        if(curr == startOfFree) startOfFree = curr->nextFree;
        //block splitting
        if(size + sizeof(metadata_t) < curr->size) {
          metadata_t* split = (void *)curr + sizeof(metadata_t) + size;
          split->isUsed = 0;
          split->size = curr->size - size - sizeof(metadata_t);
          split->nextBlock = curr->nextBlock;
          curr->nextBlock = split;
          split->nextFree = curr->nextFree;
          curr->nextFree = NULL;
          if(prevFree->nextFree == curr) prevFree->nextFree = split;
          else startOfFree = split; //edge case splitting first block
          curr->size = size;
        }
        return (void *)curr + sizeof(metadata_t);
      }
      prevFree = curr;
      curr = curr->nextFree;
    }
  }
  metadata_t * meta = sbrk( sizeof(metadata_t) );
  meta->size = size;
  meta->isUsed = 1;
  meta->nextBlock = NULL;
  meta->nextFree = NULL;
  if(lastBlock) lastBlock->nextBlock = meta;
  lastBlock = meta;

  // Allocate heap memory for the requested memory:
  void *ptr = sbrk( size );
  if(ptr == (void *)-1) return NULL;

  // Return the pointer for the requested memory:
  return ptr;
}


/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
  // Find the metadata located immediately before `ptr`:
  metadata_t *meta = ptr - sizeof( metadata_t );
  meta->isUsed = 0;
  if(startOfFree == NULL) {
    startOfFree = meta;
  } else {
    if(startOfFree > ptr) {
      meta->nextFree = startOfFree;
      startOfFree = meta;
    } else {
      metadata_t *curr = startOfFree;
      while(curr->nextFree && curr->nextFree < ptr) {
        curr = curr->nextFree;
      }
      meta->nextFree = curr->nextFree;
      curr->nextFree = meta;
    }
  }

  metadata_t * curr = startOfFree;
  while(curr) {
    if(curr->nextBlock) {
      metadata_t * next = curr->nextBlock;
      if(curr->isUsed == 0 && next->isUsed == 0) {
        //combine!!
        curr->size += next->size + sizeof(metadata_t);
        curr->nextBlock = next->nextBlock;
        curr->nextFree = next->nextFree;
      }
    }
    curr = curr->nextFree;
  }

  // printHeap();
}


/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
  if(ptr == NULL)
    return malloc(size);
  else if(size == 0){
    free(ptr);
    return NULL;
  }

  // implement realloc:
  metadata_t *meta = ptr - sizeof( metadata_t );
  if(meta->size < size) {
    free(ptr);
    void * data = malloc(size);
    memcpy(data, ptr, meta->size);
    return data;
  } else {
    metadata_t *split = ptr + size;
    split->isUsed = 0;
    split->nextBlock = meta->nextBlock;
    split->size = meta->size - size;
    meta->size = size;
    meta->nextBlock = split;
    return ptr;
  }
}
