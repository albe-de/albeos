#ifndef KERNEL_MALLOC_H
#define KERNEL_MALLOC_H

void kmalloc_init();
void* kmalloc(unsigned int size);
void kfree(void* ptr);

#endif
