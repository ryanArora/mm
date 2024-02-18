#include <stddef.h>

void slob_init(void *ptr, size_t size);
void *slob_kmalloc(size_t size);
void print_blocks(void);
