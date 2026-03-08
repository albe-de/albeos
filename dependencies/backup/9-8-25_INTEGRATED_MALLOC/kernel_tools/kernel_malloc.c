#include "including/kernel_malloc.h"

#define HEAP_SIZE 0x200000  // 2 MB heap
#define ALIGNMENT 8          // 8-byte alignment

typedef struct block_header {
    unsigned int size;
    int free;
    struct block_header* next;
} block_header;

static unsigned char heap[HEAP_SIZE];
static block_header* free_list = 0;

// Align size to nearest multiple of ALIGNMENT
static unsigned int align_up(unsigned int size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void kmalloc_init() {
    free_list = (block_header*)heap;
    free_list->size = HEAP_SIZE - sizeof(block_header);
    free_list->free = 1;
    free_list->next = 0;
}

void* kmalloc(unsigned int size) {
    size = align_up(size);
    block_header* curr = free_list;

    while (curr) {
        if (curr->free && curr->size >= size) {
            // Split block if large enough
            if (curr->size > size + sizeof(block_header)) {
                block_header* new_block = (block_header*)((unsigned char*)curr + sizeof(block_header) + size);
                new_block->size = curr->size - size - sizeof(block_header);
                new_block->free = 1;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (unsigned char*)curr + sizeof(block_header);
        }
        curr = curr->next;
    }
    return 0; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;

    block_header* block = (block_header*)((unsigned char*)ptr - sizeof(block_header));
    block->free = 1;

    // Merge with next block if free
    if (block->next && block->next->free) {
        block->size += sizeof(block_header) + block->next->size;
        block->next = block->next->next;
    }
}
