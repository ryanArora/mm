#include "slob.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SLOB_BLOCK_OWNER_FREE	0xFFFFFFFFFFFFFFFF
#define SLOB_BLOCK_OWNER_KERNEL 0xFFFFFFFFFFFFFFFE

typedef struct slob_block {
	uint64_t owner;
	void *ptr;
	size_t size;
} slob_block;

#define SLOB_MAX_BLOCKS 1024

static size_t blocks_len = 0;
static slob_block blocks[SLOB_MAX_BLOCKS];

void slob_init(void *ptr, size_t size) {
	slob_block block = {
		.owner = SLOB_BLOCK_OWNER_FREE,
		.ptr   = ptr,
		.size  = size,
	};
	blocks[0] = block;
	blocks_len += 1;
}

void *slob_kmalloc(size_t size) {
	if (blocks_len == SLOB_MAX_BLOCKS)
		return NULL;

	if (size == 0)
		return NULL;

	for (size_t i = 0; i < blocks_len; ++i) {
		slob_block block = blocks[i];
		if (block.owner != SLOB_BLOCK_OWNER_FREE)
			continue;
		if (block.size < size)
			continue;

		if (block.size == size) {
			block.size	= size;
			block.owner = SLOB_BLOCK_OWNER_KERNEL;
			return block.ptr;
		}

		memmove(blocks + i + 1, blocks + i + 2,
				sizeof(block) * (blocks_len - i));
		blocks_len += 1;

		slob_block taken_block = {
			.owner = SLOB_BLOCK_OWNER_KERNEL,
			.ptr   = block.ptr,
			.size  = size,
		};

		slob_block free_block = {
			.owner = SLOB_BLOCK_OWNER_FREE,
			.ptr   = (void *)((char *)block.ptr + size),
			.size  = block.size - size,
		};

		blocks[i]	  = taken_block;
		blocks[i + 1] = free_block;

		return taken_block.ptr;
	}

	return NULL;
}

static void print_block(slob_block block) {
	printf("owner=%s, ptr=0x%08lX, size=%zu\n",
		   block.owner == SLOB_BLOCK_OWNER_FREE ? "free " : "taken",
		   (uint64_t)block.ptr, block.size);
}

void print_blocks(void) {
	for (size_t i = 0; i < blocks_len; ++i)
		print_block(blocks[i]);
	printf("\n");
}
