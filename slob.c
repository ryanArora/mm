#include "slob.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((noreturn)) static void panic() {
	fprintf(stderr, "kernel panic\n");
	exit(EXIT_FAILURE);
}

static void segfault() {
	fprintf(stderr, "segmentation fault (core dumped)\n");
}

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

void *slob_malloc(uint64_t owner, size_t size) {
	if (owner == SLOB_BLOCK_OWNER_FREE) {
		panic();
	}

	if (blocks_len == SLOB_MAX_BLOCKS) {
		segfault();
		return NULL;
	}

	if (size == 0) {
		segfault();
		return NULL;
	}

	for (size_t i = 0; i < blocks_len; ++i) {
		slob_block block = blocks[i];

		if (block.owner != SLOB_BLOCK_OWNER_FREE)
			continue;

		if (block.size < size)
			continue;

		if (block.size == size) {
			blocks[i].owner = owner;
			return block.ptr;
		}

		memmove(blocks + i + 1, blocks + i + 2,
				sizeof(slob_block) * (blocks_len - i - 1));
		blocks_len += 1;

		slob_block taken_block = {
			.owner = owner,
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

	segfault();
	return NULL;
}

void slob_free(uint64_t owner, void *ptr) {
	if (owner == SLOB_BLOCK_OWNER_FREE) {
		panic();
	}

	/* ptr is binary searchable */
	for (size_t i = 0; i < blocks_len; ++i) {
		if (blocks[i].owner == owner && blocks[i].ptr == ptr) {
			/* Merge right and left blocks */
			if (i > 0 && i < SLOB_MAX_BLOCKS &&
				blocks[i - 1].owner == SLOB_BLOCK_OWNER_FREE &&
				blocks[i + 1].owner == SLOB_BLOCK_OWNER_FREE) {
				/* Merge left and right blocks */
				blocks[i - 1].size += blocks[i].size + blocks[i + 1].size;

				memmove(blocks + i + 2, blocks + i,
						sizeof(slob_block) * (blocks_len - i - 2));
				blocks_len -= 2;

				return;
			}

			if (i > 0 && blocks[i - 1].owner == SLOB_BLOCK_OWNER_FREE) {
				/* Merge left block */
				blocks[i - 1].size += blocks[i].size;

				memmove(blocks + i + 1, blocks + i,
						sizeof(slob_block) * (blocks_len - i - 1));
				blocks_len -= 1;

				return;
			}

			if (i < SLOB_MAX_BLOCKS &&
				blocks[i + 1].owner == SLOB_BLOCK_OWNER_FREE) {
				/* Merge right block */
				blocks[i].size += blocks[i + 1].size;

				memmove(blocks + i + 2, blocks + i + 1,
						sizeof(slob_block) * (blocks_len - i - 1));
				blocks_len -= 1;

				return;
			}

			blocks[i].owner = SLOB_BLOCK_OWNER_FREE;
			return;
		}
	}

	segfault();
	return;
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
