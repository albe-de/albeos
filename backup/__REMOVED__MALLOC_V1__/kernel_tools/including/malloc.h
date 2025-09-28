#ifndef MALLOC_H
#define MALLOC_H

#define MEMORY_POOL_SIZE (1024 * 1024)

typedef struct Block {
    unsigned int size;
    struct Block* next;
} Block;

extern Block* free_list;   // shared global
extern char memory_pool[]; // shared pool

void initialize_memory();
void* my_malloc(unsigned int size);
void my_free(void* ptr);

#endif
