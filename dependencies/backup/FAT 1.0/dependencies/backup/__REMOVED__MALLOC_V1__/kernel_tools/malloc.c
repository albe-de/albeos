#include "including/malloc.h"
#include "including/vega.h"

char memory_pool[MEMORY_POOL_SIZE];        // actual storage
Block* free_list = (Block*)memory_pool;    // real shared free list

void initialize_memory() {
    free_list->size = MEMORY_POOL_SIZE - sizeof(Block);
    free_list->next = 0;

    write("Memory pool initialized. Size of first block: ");
    write_int(free_list->size);
    write("\n");
}


void* my_malloc(unsigned int size) {
    if (size == 0) return 0;

    if (size % 4 != 0) {
        size += 4 - (size % 4);
    }

    Block* current = free_list;
    Block* previous = 0;

    write("Requested size: "); // Debug statement
    write_int(size); // Assuming you have a function to print integers
    write("\n");

    while (current) {
        //write("Checking block of size: "); // Debug statement
        //write_int(current->size); // Print the size of the current block
        //write("\n");

        if (current->size >= size) {
            if (current->size >= size + sizeof(Block) + 4) {
                Block* new_block = (Block*)((char*)current + sizeof(Block) + size);
                new_block->size = current->size - size - sizeof(Block);
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;

                write("Allocated block of size: "); // Debug statement
                write_int(current->size);
                write("\n");

                // Return the pointer to the allocated memory
                return (char*)current + sizeof(Block);
            } else {
                // Remove the block from the free list
                if (previous) {
                    previous->next = current->next;
                } else {
                    free_list = current->next; // Update head of the free list
                }
                write("Using entire block of size: "); // Debug statement
                write_int(current->size);
                write("\n");

                // Return the pointer to the allocated memory
                return (char*)current + sizeof(Block);
            }
        }
        previous = current;
        current = current->next;
    }
    write("Allocation failed: No suitable block found.\n"); // Debug statement
    return 0; // No suitable block found
}


// kills block
void my_free(void* ptr) {
    if (!ptr) return;

    Block* block_to_free = (Block*)((char*)ptr - sizeof(Block));
    block_to_free->next = free_list;
    free_list = block_to_free;
}
