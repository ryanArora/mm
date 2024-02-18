#include <stddef.h>
#include <stdint.h>

#define SLOB_BLOCK_OWNER_FREE	0xFFFFFFFFFFFFFFFF
#define SLOB_BLOCK_OWNER_KERNEL 0xFFFFFFFFFFFFFFFE

void slob_init(void *ptr, size_t size);
void *slob_malloc(uint64_t owner, size_t size);
void slob_free(uint64_t owner, void *ptr);

void print_blocks(void);
