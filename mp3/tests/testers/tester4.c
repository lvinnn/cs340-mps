#include "tester-utils.h"

#define START_MALLOC_SIZE (128 * M)
#define STOP_MALLOC_SIZE (1 * K)

void *reduce(void *ptr, int size) {
    if (size > STOP_MALLOC_SIZE) {
        void *ptr1 = realloc(ptr, size / 2);
        void *ptr2 = malloc(size / 2);

        if (ptr1 == NULL || ptr2 == NULL) {
            fprintf(stderr, "Memory failed to allocate!\n");
            exit(1);
        }

        // printf("pre-reduce size: %d\n", size);
        ptr1 = reduce(ptr1, size / 2);
        // printf("ptr 1: %d\n", *((int *)ptr1));
        ptr2 = reduce(ptr2, size / 2);
        // printf("ptr 2: %d\n", *((int *)ptr2));

        if (*((int *)ptr1) != size / 2 || *((int *)ptr2) != size / 2) {
            printf("oi amigo, size/2 = %d, but *((int *)ptr1) = %d, *((int *)ptr2) = %d\n, ", size/2, *((int *)ptr1), *((int *)ptr2));
            fprintf(stderr, "Memory failed to contain correct data after many "
                            "allocations!\n");
            exit(2);
        }

        free(ptr2);
        ptr1 = realloc(ptr1, size);

        if (*((int *)ptr1) != size / 2) {
            fprintf(stderr,
                    "Memory failed to contain correct data after realloc()!\n");
            exit(3);
        }

        *((int *)ptr1) = size;
        // printf("ptr1 is now %d, %d\n", size, *((int *)ptr1));
        return ptr1;
    } else {
        // printf("size: %d\n", size);
        *((int *)ptr) = size;
        return ptr;
    }
}

int main() {
    (void) malloc(1);

    int size = START_MALLOC_SIZE;
    while (size > STOP_MALLOC_SIZE) {
        void *ptr = malloc(size);
        ptr = reduce(ptr, size / 2);
        free(ptr);
        
        size /= 2;
    }

    fprintf(stderr, "Memory was allocated, used, and freed!\n");
    return 0;
}
