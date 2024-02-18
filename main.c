#include "slob.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	slob_init(0x00000000, 32 * 3);
	print_blocks();

	void *p1 = slob_malloc(SLOB_BLOCK_OWNER_KERNEL, 32);
	void *p2 = slob_malloc(SLOB_BLOCK_OWNER_KERNEL, 32);
	void *p3 = slob_malloc(SLOB_BLOCK_OWNER_KERNEL, 32);
	print_blocks();

	slob_free(SLOB_BLOCK_OWNER_KERNEL, p1);
	print_blocks();
	slob_free(SLOB_BLOCK_OWNER_KERNEL, p3);
	print_blocks();
	slob_free(SLOB_BLOCK_OWNER_KERNEL, p2);
	print_blocks();
	return EXIT_SUCCESS;
}
